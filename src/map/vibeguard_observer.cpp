#include "vibeguard_observer.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <deque>
#include <fstream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <common/showmsg.hpp>
#include <common/socket.hpp>
#include <common/sql.hpp>
#include <common/timer.hpp>

#include "battle.hpp"
#include "clif.hpp"
#include "map.hpp"
#include "npc.hpp"
#include "pc.hpp"

namespace {

constexpr size_t SAMPLE_COUNT = 24;
constexpr int32 INITIAL_WARNING_SCORE = 30;
constexpr int32 PERSISTENT_WARNING_SCORE = 60;
constexpr t_tick LINK_RECHECK_MS = 30000;
constexpr t_tick WARNING_COOLDOWN_MS = 1800000;
constexpr t_tick ENFORCEMENT_INTERVAL_MS = 5000;
constexpr t_tick COMBINED_SIGNAL_WINDOW_MS = 120000;
constexpr t_tick COMBINED_SCORE_COOLDOWN_MS = 30000;
constexpr int32 SCORE_DECAY_PER_MINUTE = 2;
constexpr size_t SCORED_ACTION_COUNT = 3;

struct TimingWindow {
	t_tick last_tick = 0;
	std::deque<int32> intervals;
};

struct ObserverState {
	std::array<TimingWindow, 4> timings;
	std::array<t_tick, SCORED_ACTION_COUNT> last_regular_signal {};
	int32 score = 0;
	t_tick last_decay_tick = 0;
	t_tick last_combined_score_tick = 0;
	t_tick last_persistent_warning = 0;
	bool initial_warning_sent = false;
};

struct EnforcementState {
	t_tick first_missing_tick = 0;
	bool warning_sent = false;
};

std::unordered_map<uint32, ObserverState> observer_states;
std::unordered_map<uint32, t_tick> linked_accounts;
std::unordered_map<uint32, EnforcementState> enforcement_states;

void write_suspicion_log(const map_session_data& sd, const char* event, int32 score) {
	std::ofstream log("./log/vibeguard-suspicion.log", std::ios::app);
	if (!log.is_open())
		return;
	const auto now = std::time(nullptr);
	std::tm utc {};
#ifdef _WIN32
	gmtime_s(&utc, &now);
#else
	gmtime_r(&now, &utc);
#endif
	log << std::put_time(&utc, "%Y-%m-%dT%H:%M:%SZ")
		<< " event=" << event
		<< " account=" << sd.status.account_id
		<< " char=" << sd.status.char_id
		<< " score=" << score
		<< " observation=true\n";
}

void persist_score(const map_session_data& sd, int32 score, bool warning) {
	if (SQL_ERROR == Sql_Query(mmysql_handle,
		"UPDATE `vibeguard_sessions` SET `suspicion_score`='%d'%s "
		"WHERE `account_id`='%u' AND `state`='open' ORDER BY `opened_at` DESC LIMIT 1",
		score, warning ? ", `last_warning_at`=UTC_TIMESTAMP(6)" : "", sd.status.account_id))
		Sql_ShowDebug(mmysql_handle);
}

void decay_score(map_session_data& sd, ObserverState& state, t_tick tick) {
	if (state.last_decay_tick == 0) {
		state.last_decay_tick = tick;
		return;
	}
	if (DIFF_TICK(tick, state.last_decay_tick) < 60000)
		return;
	const auto minutes = static_cast<int32>(DIFF_TICK(tick, state.last_decay_tick) / 60000);
	const auto previous_score = state.score;
	state.score = std::max(0, state.score - minutes * SCORE_DECAY_PER_MINUTE);
	state.last_decay_tick += minutes * 60000;
	if (state.score != previous_score)
		persist_score(sd, state.score, false);
	if (state.score < INITIAL_WARNING_SCORE)
		state.initial_warning_sent = false;
}

bool active_link(uint32 account_id, t_tick tick) {
	const auto cached = linked_accounts.find(account_id);
	if (cached != linked_accounts.end() && DIFF_TICK(cached->second, tick) > 0)
		return true;

	if (SQL_ERROR == Sql_Query(mmysql_handle,
		"SELECT `session_id` FROM `vibeguard_sessions` WHERE `account_id`='%u' "
		"AND `state`='open' AND `last_heartbeat` >= UTC_TIMESTAMP(6) - INTERVAL %d SECOND LIMIT 1",
		account_id, battle_config.vibeguard_heartbeat_ttl_seconds)) {
		Sql_ShowDebug(mmysql_handle);
		linked_accounts.erase(account_id);
		return false;
	}
	const bool active = Sql_NumRows(mmysql_handle) > 0;
	Sql_FreeResult(mmysql_handle);
	if (active)
		linked_accounts[account_id] = tick + LINK_RECHECK_MS;
	else
		linked_accounts.erase(account_id);
	return active;
}

TIMER_FUNC(vibeguard_enforcement_timer) {
	const bool enforce_session = battle_config.vibeguard_enforcement_mode > 0;
	if (!enforce_session)
		enforcement_states.clear();

	std::unordered_set<uint32> online_accounts;
	auto* iterator = mapit_getallusers();
	for (auto* sd = static_cast<map_session_data*>(mapit_first(iterator));
		mapit_exists(iterator);
		sd = static_cast<map_session_data*>(mapit_next(iterator))) {
		if (sd == nullptr || sd->fd <= 0)
			continue;
		const auto account_id = sd->status.account_id;
		online_accounts.insert(account_id);
		const auto observer = observer_states.find(account_id);
		if (observer != observer_states.end())
			decay_score(*sd, observer->second, tick);
		if (!enforce_session)
			continue;
		if (pc_get_group_level(sd) >= battle_config.vibeguard_exempt_group_level) {
			enforcement_states.erase(account_id);
			continue;
		}
		if (active_link(account_id, tick)) {
			enforcement_states.erase(account_id);
			continue;
		}

		auto& state = enforcement_states[account_id];
		if (state.first_missing_tick == 0)
			state.first_missing_tick = tick;
		const auto elapsed_seconds = static_cast<int32>(DIFF_TICK(tick, state.first_missing_tick) / 1000);
		if (!state.warning_sent && elapsed_seconds >= battle_config.vibeguard_warning_seconds) {
			const auto remaining = std::max(0,
				battle_config.vibeguard_grace_seconds - elapsed_seconds);
			char message[220];
			safesnprintf(message, sizeof(message),
				"[VibeGuard] Sessao de protecao ausente. Inicie pelo launcher; desconexao em %d segundos se o vinculo nao for restaurado.",
				remaining);
			clif_messagecolor(sd, color_table[COLOR_RED], message, false, SELF);
			write_suspicion_log(*sd, "session_missing_warning", 0);
			state.warning_sent = true;
		}
		if (battle_config.vibeguard_enforcement_mode >= 2
			&& elapsed_seconds >= battle_config.vibeguard_grace_seconds) {
			write_suspicion_log(*sd, "session_missing_disconnect", 0);
			clif_messagecolor(sd, color_table[COLOR_RED],
				"[VibeGuard] Sessao de protecao nao confirmada. Reconecte usando o launcher oficial.",
				false, SELF);
			clif_authfail_fd(sd->fd, 15);
		}
	}
	mapit_free(iterator);

	for (auto state = enforcement_states.begin(); state != enforcement_states.end();) {
		if (online_accounts.find(state->first) == online_accounts.end())
			state = enforcement_states.erase(state);
		else
			++state;
	}
	for (auto state = observer_states.begin(); state != observer_states.end();) {
		if (online_accounts.find(state->first) == online_accounts.end())
			state = observer_states.erase(state);
		else
			++state;
	}
	for (auto account = linked_accounts.begin(); account != linked_accounts.end();) {
		if (online_accounts.find(account->first) == online_accounts.end())
			account = linked_accounts.erase(account);
		else
			++account;
	}
	return 0;
}

void show_initial_warning(map_session_data& sd, ObserverState& state) {
	clif_messagecolor(&sd, color_table[COLOR_RED],
		"[VibeGuard] Padrao automatizado suspeito detectado. Continue jogando manualmente; reincidencia pode gerar verificacao e punicao apos revisao.",
		false, SELF);
	npc_event(&sd, "VibeGuardWarning::OnSuspicion", 0);
	state.initial_warning_sent = true;
	write_suspicion_log(sd, "warning_bottom_dialog", state.score);
	persist_score(sd, state.score, true);
}

void show_persistent_warning(map_session_data& sd, ObserverState& state, t_tick tick) {
	clif_messagecolor(&sd, color_table[COLOR_RED],
		"[VibeGuard] Suspeita persistente. Sua atividade foi marcada para revisao; automacao proibida pode resultar em punicao.",
		false, SELF);
	npc_event(&sd, "VibeGuardWarning::OnPersistentSuspicion", 0);
	state.last_persistent_warning = tick;
	write_suspicion_log(sd, "warning_dialog", state.score);
	persist_score(sd, state.score, true);
}

const char* action_name(e_vibeguard_action action) {
	switch (action) {
		case e_vibeguard_action::MOVE: return "move";
		case e_vibeguard_action::ATTACK: return "attack";
		case e_vibeguard_action::PICKUP: return "pickup";
		case e_vibeguard_action::SKILL: return "skill";
	}
	return "unknown";
}

void evaluate_window(map_session_data& sd, ObserverState& state, TimingWindow& window,
	e_vibeguard_action action, t_tick tick) {
	if (window.intervals.size() < SAMPLE_COUNT)
		return;

	double sum = 0.0;
	for (const auto interval : window.intervals)
		sum += interval;
	const double mean = sum / window.intervals.size();
	double variance = 0.0;
	for (const auto interval : window.intervals) {
		const double delta = interval - mean;
		variance += delta * delta;
	}
	variance /= window.intervals.size();
	const double coefficient = mean > 0.0 ? std::sqrt(variance) / mean : 1.0;

	if (mean >= 150.0 && mean <= 8000.0 && coefficient < 0.035) {
		const auto action_index = static_cast<size_t>(action);
		state.last_regular_signal[action_index] = tick;
		std::string signal_event = "regular_";
		signal_event += action_name(action);
		write_suspicion_log(sd, signal_event.c_str(), state.score);

		int32 combined_signals = 0;
		for (const auto signal_tick : state.last_regular_signal) {
			if (signal_tick != 0 && DIFF_TICK(tick, signal_tick) <= COMBINED_SIGNAL_WINDOW_MS)
				++combined_signals;
		}
		const bool cooldown_elapsed = state.last_combined_score_tick == 0
			|| DIFF_TICK(tick, state.last_combined_score_tick) >= COMBINED_SCORE_COOLDOWN_MS;
		if (combined_signals >= 2 && cooldown_elapsed) {
			state.score = std::min(100, state.score + (combined_signals >= 3 ? 15 : 8));
			state.last_combined_score_tick = tick;
			write_suspicion_log(sd,
				combined_signals >= 3 ? "combined_automation_3" : "combined_automation_2",
				state.score);
			persist_score(sd, state.score, false);
		}
	}

	for (size_t index = 0; index < SAMPLE_COUNT / 2; ++index)
		window.intervals.pop_front();

	if (state.score >= INITIAL_WARNING_SCORE && !state.initial_warning_sent)
		show_initial_warning(sd, state);
	if (state.score >= PERSISTENT_WARNING_SCORE
		&& (state.last_persistent_warning == 0 || DIFF_TICK(tick, state.last_persistent_warning) >= WARNING_COOLDOWN_MS))
		show_persistent_warning(sd, state, tick);
}

} // namespace

bool vibeguard_bind_session(map_session_data& sd, const char* pairing_code) {
	if (SQL_ERROR == Sql_Query(mmysql_handle,
		"UPDATE `vibeguard_sessions` SET `account_id`='%u', `char_id`='%u', `linked_at`=UTC_TIMESTAMP(6) "
		"WHERE `pairing_code`='%s' AND `state`='open' "
		"AND `last_heartbeat` >= UTC_TIMESTAMP(6) - INTERVAL 60 SECOND "
		"AND (`account_id` IS NULL OR `account_id`='%u') LIMIT 1",
		sd.status.account_id, sd.status.char_id, pairing_code, sd.status.account_id)) {
		Sql_ShowDebug(mmysql_handle);
		return false;
	}
	if (Sql_NumRowsAffected(mmysql_handle) == 0)
		return false;
	linked_accounts[sd.status.account_id] = gettick() + LINK_RECHECK_MS;
	observer_states.erase(sd.status.account_id);
	write_suspicion_log(sd, "linked", 0);
	return true;
}

void vibeguard_observe_action(map_session_data& sd, e_vibeguard_action action) {
	// Skill repetition and autopot are accepted quality-of-life behavior on VibeRO.
	// They must never contribute evidence or suspicion score.
	if (action == e_vibeguard_action::SKILL)
		return;
	if (pc_get_group_level(&sd) > 0)
		return;
	const auto tick = gettick();
	if (!active_link(sd.status.account_id, tick))
		return;

	auto& state = observer_states[sd.status.account_id];
	decay_score(sd, state, tick);

	auto& window = state.timings[static_cast<size_t>(action)];
	if (window.last_tick != 0) {
		const auto interval = static_cast<int32>(DIFF_TICK(tick, window.last_tick));
		if (interval >= 100 && interval <= 10000) {
			window.intervals.push_back(interval);
			if (window.intervals.size() > SAMPLE_COUNT)
				window.intervals.pop_front();
		} else if (interval > 10000) {
			window.intervals.clear();
		}
	}
	window.last_tick = tick;
	evaluate_window(sd, state, window, action, tick);
}

void vibeguard_show_status(map_session_data& sd) {
	const auto iterator = observer_states.find(sd.status.account_id);
	const auto score = iterator == observer_states.end() ? 0 : iterator->second.score;
	char message[160];
	safesnprintf(message, sizeof(message), "[VibeGuard] Sessao vinculada: %s | pontuacao observacional: %d/100",
		active_link(sd.status.account_id, gettick()) ? "sim" : "nao", score);
	clif_displaymessage(sd.fd, message);
}

void vibeguard_test_warning(map_session_data& sd, bool persistent) {
	auto& state = observer_states[sd.status.account_id];
	state.score = persistent ? PERSISTENT_WARNING_SCORE : INITIAL_WARNING_SCORE;
	if (persistent)
		show_persistent_warning(sd, state, gettick());
	else
		show_initial_warning(sd, state);
}

void vibeguard_observer_init() {
	add_timer_func_list(vibeguard_enforcement_timer, "vibeguard_enforcement_timer");
	add_timer_interval(gettick() + ENFORCEMENT_INTERVAL_MS,
		vibeguard_enforcement_timer, 0, 0, ENFORCEMENT_INTERVAL_MS);
}

void vibeguard_observer_final() {
	observer_states.clear();
	linked_accounts.clear();
	enforcement_states.clear();
}
