// VibeGuard observation-only session endpoints.

#include "vibeguard_controller.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

#include <common/showmsg.hpp>
#include <common/sql.hpp>

#include "sqllock.hpp"
#include "web.hpp"

namespace {

using Clock = std::chrono::steady_clock;

struct ObservationSession {
	std::string id;
	std::string pairing_code;
	std::string client_version;
	std::string manifest_sha256;
	uint64 last_sequence = 0;
	uint32 account_id = 0;
	std::time_t opened_unix = 0;
	Clock::time_point expires_at;
};

std::unordered_map<std::string, ObservationSession> sessions;
std::mutex sessions_mutex;
std::mutex observation_log_mutex;
constexpr size_t MAX_SESSIONS = 4096;

bool execute_map_query(const char* query) {
	SQLLock lock(MAP_SQL_LOCK);
	lock.lock();
	auto* handle = lock.getHandle();
	const auto result = Sql_Query(handle, query);
	if (result == SQL_ERROR)
		Sql_ShowDebug(handle);
	lock.unlock();
	return result == SQL_SUCCESS;
}

bool persist_open(const ObservationSession& session) {
	SQLLock lock(MAP_SQL_LOCK);
	lock.lock();
	auto* handle = lock.getHandle();
	const auto result = Sql_Query(handle,
		"INSERT INTO `vibeguard_sessions` "
		"(`session_id`,`pairing_code`,`client_version`,`manifest_sha256`,`state`,`opened_at`,`last_heartbeat`) "
		"VALUES ('%s','%s','%s','%s','open',UTC_TIMESTAMP(6),UTC_TIMESTAMP(6))",
		session.id.c_str(), session.pairing_code.c_str(), session.client_version.c_str(), session.manifest_sha256.c_str());
	if (result == SQL_ERROR)
		Sql_ShowDebug(handle);
	lock.unlock();
	return result == SQL_SUCCESS;
}

void persist_heartbeat(const ObservationSession& session) {
	SQLLock lock(MAP_SQL_LOCK);
	lock.lock();
	auto* handle = lock.getHandle();
	if (SQL_ERROR == Sql_Query(handle,
		"UPDATE `vibeguard_sessions` SET `last_heartbeat`=UTC_TIMESTAMP(6), `state`='open' "
		"WHERE `session_id`='%s' LIMIT 1", session.id.c_str()))
		Sql_ShowDebug(handle);
	lock.unlock();
}

void persist_state(const ObservationSession& session, const char* state) {
	SQLLock lock(MAP_SQL_LOCK);
	lock.lock();
	auto* handle = lock.getHandle();
	if (SQL_ERROR == Sql_Query(handle,
		"UPDATE `vibeguard_sessions` SET `state`='%s', `closed_at`=UTC_TIMESTAMP(6) "
		"WHERE `session_id`='%s' LIMIT 1", state, session.id.c_str()))
		Sql_ShowDebug(handle);
	lock.unlock();
}

void json_response(Response& response, int32 status, const nlohmann::json& body) {
	response.status = status;
	response.set_header("Cache-Control", "no-store");
	response.set_content(body.dump(), "application/json");
}

void error_response(Response& response, int32 status, const char* code) {
	json_response(response, status, {
		{ "ok", false },
		{ "error", code },
		{ "observationMode", true }
	});
}

std::string lower_ascii(std::string value) {
	std::transform(value.begin(), value.end(), value.begin(), [](unsigned char character) {
		return static_cast<char>(std::tolower(character));
	});
	return value;
}

bool valid_hex(const std::string& value, size_t expected_length) {
	return value.size() == expected_length && std::all_of(value.begin(), value.end(), [](unsigned char character) {
		return std::isxdigit(character) != 0;
	});
}

bool valid_version(const std::string& value) {
	return !value.empty() && value.size() <= 32 && std::all_of(value.begin(), value.end(), [](unsigned char character) {
		return std::isalnum(character) != 0 || character == '.' || character == '-' || character == '_';
	});
}

bool valid_account_name(const std::string& value) {
	return value.size() >= 4 && value.size() <= 24
		&& std::all_of(value.begin(), value.end(), [](unsigned char character) {
			return character >= 0x21 && character <= 0x7e;
		});
}

bool resolve_account_id(const std::string& account_name, uint32& account_id) {
	SQLLock lock(LOGIN_SQL_LOCK);
	lock.lock();
	auto* handle = lock.getHandle();
	char escaped_name[49];
	Sql_EscapeStringLen(handle, escaped_name, account_name.c_str(), account_name.size());
	if (SQL_ERROR == Sql_Query(handle,
		"SELECT `account_id` FROM `login` WHERE `userid`='%s' LIMIT 1", escaped_name)) {
		Sql_ShowDebug(handle);
		lock.unlock();
		return false;
	}
	if (Sql_NumRows(handle) != 1 || SQL_SUCCESS != Sql_NextRow(handle)) {
		Sql_FreeResult(handle);
		lock.unlock();
		return false;
	}
	char* data = nullptr;
	Sql_GetData(handle, 0, &data, nullptr);
	account_id = static_cast<uint32>(strtoul(data, nullptr, 10));
	Sql_FreeResult(handle);
	lock.unlock();
	return account_id != 0;
}

bool claim_account(const ObservationSession& session, uint32 account_id) {
	SQLLock lock(MAP_SQL_LOCK);
	lock.lock();
	auto* handle = lock.getHandle();
	if (SQL_ERROR == Sql_Query(handle,
		"UPDATE `vibeguard_sessions` SET `state`='superseded', `closed_at`=UTC_TIMESTAMP(6) "
		"WHERE `account_id`='%u' AND `state`='open' AND `session_id`<>'%s'",
		account_id, session.id.c_str())) {
		Sql_ShowDebug(handle);
		lock.unlock();
		return false;
	}
	const auto result = Sql_Query(handle,
		"UPDATE `vibeguard_sessions` SET `account_id`='%u', `char_id`=NULL, `linked_at`=UTC_TIMESTAMP(6) "
		"WHERE `session_id`='%s' AND `state`='open' LIMIT 1",
		account_id, session.id.c_str());
	if (result == SQL_ERROR)
		Sql_ShowDebug(handle);
	const auto changed = result == SQL_SUCCESS && Sql_NumRowsAffected(handle) == 1;
	lock.unlock();
	return changed;
}

std::vector<uint32> recent_accounts_for_address(const std::string& address, std::time_t opened_unix) {
	std::vector<uint32> accounts;
	if (address.empty() || address.size() > 100)
		return accounts;
	SQLLock lock(LOGIN_SQL_LOCK);
	lock.lock();
	auto* handle = lock.getHandle();
	char escaped_address[201];
	Sql_EscapeStringLen(handle, escaped_address, address.c_str(), address.size());
	if (SQL_ERROR == Sql_Query(handle,
		"SELECT `account_id` FROM `login` WHERE `last_ip`='%s' "
		"AND `lastlogin`>=FROM_UNIXTIME('%lld') ORDER BY `lastlogin` ASC LIMIT 20",
		escaped_address, static_cast<long long>(opened_unix))) {
		Sql_ShowDebug(handle);
		lock.unlock();
		return accounts;
	}
	while (SQL_SUCCESS == Sql_NextRow(handle)) {
		char* data = nullptr;
		Sql_GetData(handle, 0, &data, nullptr);
		const auto account_id = static_cast<uint32>(strtoul(data, nullptr, 10));
		if (account_id != 0)
			accounts.push_back(account_id);
	}
	Sql_FreeResult(handle);
	lock.unlock();
	return accounts;
}

bool auto_claim_account(const ObservationSession& session, uint32 account_id) {
	SQLLock lock(MAP_SQL_LOCK);
	lock.lock();
	auto* handle = lock.getHandle();
	if (SQL_ERROR == Sql_Query(handle,
		"SELECT 1 FROM `vibeguard_sessions` WHERE `account_id`='%u' AND `state`='open' "
		"AND `session_id`<>'%s' LIMIT 1", account_id, session.id.c_str())) {
		Sql_ShowDebug(handle);
		lock.unlock();
		return false;
	}
	const auto already_claimed = Sql_NumRows(handle) > 0;
	Sql_FreeResult(handle);
	if (already_claimed) {
		lock.unlock();
		return false;
	}
	const auto result = Sql_Query(handle,
		"UPDATE `vibeguard_sessions` SET `account_id`='%u', `char_id`=NULL, `linked_at`=UTC_TIMESTAMP(6) "
		"WHERE `session_id`='%s' AND `state`='open' AND `account_id` IS NULL LIMIT 1",
		account_id, session.id.c_str());
	if (result == SQL_ERROR)
		Sql_ShowDebug(handle);
	const auto changed = result == SQL_SUCCESS && Sql_NumRowsAffected(handle) == 1;
	lock.unlock();
	return changed;
}

std::string random_hex(size_t byte_count) {
	std::random_device source;
	static constexpr char digits[] = "0123456789abcdef";
	std::string result;
	result.reserve(byte_count * 2);
	for (size_t index = 0; index < byte_count; ++index) {
		const auto value = static_cast<uint8>(source());
		result.push_back(digits[value >> 4]);
		result.push_back(digits[value & 0x0f]);
	}
	return result;
}

std::string utc_timestamp() {
	const auto now = std::chrono::system_clock::now();
	const std::time_t time = std::chrono::system_clock::to_time_t(now);
	std::tm utc {};
#ifdef _WIN32
	gmtime_s(&utc, &time);
#else
	gmtime_r(&time, &utc);
#endif
	std::ostringstream output;
	output << std::put_time(&utc, "%Y-%m-%dT%H:%M:%SZ");
	return output.str();
}

void write_observation(const char* event, const ObservationSession& session) {
	std::lock_guard<std::mutex> lock(observation_log_mutex);
	const std::filesystem::path log_path(web_config.vibeguard_log_path);
	if (log_path.has_parent_path()) {
		std::error_code error;
		std::filesystem::create_directories(log_path.parent_path(), error);
		if (error) {
			ShowWarning("VibeGuard could not create observation log directory.\n");
			return;
		}
	}
	std::ofstream log(web_config.vibeguard_log_path, std::ios::app);
	if (!log.is_open()) {
		ShowWarning("VibeGuard could not open observation log.\n");
		return;
	}
	log << utc_timestamp()
		<< " event=" << event
		<< " session=" << session.id
		<< " client=" << session.client_version
		<< " manifest=" << session.manifest_sha256
		<< " observation=true\n";
}

void cleanup_expired_locked() {
	const auto now = Clock::now();
	for (auto iterator = sessions.begin(); iterator != sessions.end();) {
		if (iterator->second.expires_at <= now) {
			persist_state(iterator->second, "expired");
			write_observation("expired", iterator->second);
			iterator = sessions.erase(iterator);
		} else {
			++iterator;
		}
	}
}

std::string bearer_token(const Request& request) {
	const auto authorization = request.get_header_value("Authorization");
	static const std::string prefix = "Bearer ";
	if (authorization.size() != prefix.size() + 64 || authorization.compare(0, prefix.size(), prefix) != 0)
		return {};
	const auto token = authorization.substr(prefix.size());
	return valid_hex(token, 64) ? lower_ascii(token) : std::string {};
}

bool service_ready(Response& response) {
	if (!web_config.vibeguard_observation_enabled) {
		error_response(response, 503, "observation_disabled");
		return false;
	}
	if (!valid_hex(web_config.vibeguard_manifest_sha256, 64)) {
		error_response(response, 503, "manifest_not_configured");
		return false;
	}
	if (!web_config.vibeguard_previous_manifest_sha256.empty()
		&& !valid_hex(web_config.vibeguard_previous_manifest_sha256, 64)) {
		error_response(response, 503, "previous_manifest_invalid");
		return false;
	}
	return true;
}

} // namespace

bool vibeguard_storage_initialize() {
	if (!execute_map_query(
		"CREATE TABLE IF NOT EXISTS `vibeguard_sessions` ("
		"`session_id` CHAR(32) NOT NULL,"
		"`pairing_code` CHAR(12) NOT NULL,"
		"`account_id` INT UNSIGNED NULL,"
		"`char_id` INT UNSIGNED NULL,"
		"`client_version` VARCHAR(32) NOT NULL,"
		"`manifest_sha256` CHAR(64) NOT NULL,"
		"`state` VARCHAR(12) NOT NULL DEFAULT 'open',"
		"`suspicion_score` SMALLINT UNSIGNED NOT NULL DEFAULT 0,"
		"`opened_at` DATETIME(6) NOT NULL,"
		"`last_heartbeat` DATETIME(6) NOT NULL,"
		"`linked_at` DATETIME(6) NULL,"
		"`closed_at` DATETIME(6) NULL,"
		"`last_warning_at` DATETIME(6) NULL,"
		"PRIMARY KEY (`session_id`),"
		"UNIQUE KEY `pairing_code` (`pairing_code`),"
		"KEY `account_state` (`account_id`,`state`)"
		") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4"))
		return false;

	// Bearer tokens intentionally live only in this process. After a web-server
	// restart, previously open rows can no longer authenticate and must not block
	// a new automatic claim for the same account.
	return execute_map_query(
		"UPDATE `vibeguard_sessions` SET `state`='interrupted', "
		"`closed_at`=UTC_TIMESTAMP(6) WHERE `state`='open'");
}

HANDLER_FUNC(vibeguard_session_open) {
	if (!service_ready(res))
		return;

	nlohmann::json body;
	try {
		body = nlohmann::json::parse(req.body);
	} catch (const nlohmann::json::exception&) {
		error_response(res, 400, "invalid_json");
		return;
	}

	if (!body.is_object()
		|| !body.contains("protocolVersion") || !body["protocolVersion"].is_number_integer()
		|| !body.contains("clientVersion") || !body["clientVersion"].is_string()
		|| !body.contains("manifestSha256") || !body["manifestSha256"].is_string()) {
		error_response(res, 400, "invalid_request");
		return;
	}

	const auto protocol_version = body["protocolVersion"].get<int32>();
	const auto client_version = body["clientVersion"].get<std::string>();
	const auto manifest_hash = lower_ascii(body["manifestSha256"].get<std::string>());
	if (protocol_version != 1 || !valid_version(client_version) || !valid_hex(manifest_hash, 64)) {
		error_response(res, 400, "invalid_request");
		return;
	}
	if (manifest_hash != lower_ascii(web_config.vibeguard_manifest_sha256)
		&& (web_config.vibeguard_previous_manifest_sha256.empty()
			|| manifest_hash != lower_ascii(web_config.vibeguard_previous_manifest_sha256))) {
		error_response(res, 403, "release_not_allowed");
		return;
	}

	ObservationSession session;
	session.id = random_hex(16);
	session.pairing_code = random_hex(6);
	session.client_version = client_version;
	session.manifest_sha256 = manifest_hash;
	session.opened_unix = std::time(nullptr);
	session.expires_at = Clock::now() + std::chrono::seconds(web_config.vibeguard_session_ttl_seconds);
	const auto token = random_hex(32);

	{
		std::lock_guard<std::mutex> lock(sessions_mutex);
		cleanup_expired_locked();
		if (sessions.size() >= MAX_SESSIONS) {
			error_response(res, 429, "session_limit");
			return;
		}
		sessions.emplace(token, session);
	}
	if (!persist_open(session)) {
		std::lock_guard<std::mutex> lock(sessions_mutex);
		sessions.erase(token);
		error_response(res, 503, "storage_unavailable");
		return;
	}

	write_observation("opened", session);
	json_response(res, 201, {
		{ "ok", true },
		{ "protocolVersion", 1 },
		{ "sessionId", session.id },
		{ "pairingCode", session.pairing_code },
		{ "token", token },
		{ "heartbeatSeconds", web_config.vibeguard_heartbeat_seconds },
		{ "expiresSeconds", web_config.vibeguard_session_ttl_seconds },
		{ "observationMode", true }
	});
}

HANDLER_FUNC(vibeguard_session_heartbeat) {
	if (!service_ready(res))
		return;
	const auto token = bearer_token(req);
	if (token.empty()) {
		error_response(res, 401, "invalid_token");
		return;
	}

	nlohmann::json body;
	try {
		body = nlohmann::json::parse(req.body);
	} catch (const nlohmann::json::exception&) {
		error_response(res, 400, "invalid_json");
		return;
	}
	if (!body.is_object() || !body.contains("sequence") || !body["sequence"].is_number_unsigned()) {
		error_response(res, 400, "invalid_request");
		return;
	}

	const auto sequence = body["sequence"].get<uint64>();
	ObservationSession updated_session;
	{
		std::lock_guard<std::mutex> lock(sessions_mutex);
		cleanup_expired_locked();
		const auto iterator = sessions.find(token);
		if (iterator == sessions.end()) {
			error_response(res, 401, "session_expired");
			return;
		}
		if (sequence <= iterator->second.last_sequence) {
			error_response(res, 409, "sequence_replayed");
			return;
		}
		iterator->second.last_sequence = sequence;
		iterator->second.expires_at = Clock::now() + std::chrono::seconds(web_config.vibeguard_session_ttl_seconds);
		updated_session = iterator->second;
	}
	persist_heartbeat(updated_session);
	json_response(res, 200, {
		{ "ok", true },
		{ "sequence", sequence },
		{ "observationMode", true }
	});
}

HANDLER_FUNC(vibeguard_session_claim) {
	if (!service_ready(res))
		return;
	const auto token = bearer_token(req);
	if (token.empty()) {
		error_response(res, 401, "invalid_token");
		return;
	}

	nlohmann::json body;
	try {
		body = nlohmann::json::parse(req.body);
	} catch (const nlohmann::json::exception&) {
		error_response(res, 400, "invalid_json");
		return;
	}
	if (!body.is_object() || !body.contains("accountName") || !body["accountName"].is_string()) {
		error_response(res, 400, "invalid_request");
		return;
	}
	const auto account_name = body["accountName"].get<std::string>();
	if (!valid_account_name(account_name)) {
		error_response(res, 400, "invalid_account_name");
		return;
	}

	ObservationSession session;
	{
		std::lock_guard<std::mutex> lock(sessions_mutex);
		cleanup_expired_locked();
		const auto iterator = sessions.find(token);
		if (iterator == sessions.end()) {
			error_response(res, 401, "session_expired");
			return;
		}
		session = iterator->second;
	}

	uint32 account_id = 0;
	if (!resolve_account_id(account_name, account_id)) {
		error_response(res, 404, "account_not_found");
		return;
	}
	if (!claim_account(session, account_id)) {
		error_response(res, 409, "account_claim_failed");
		return;
	}

	write_observation("claimed", session);
	json_response(res, 200, {
		{ "ok", true },
		{ "accountBound", true },
		{ "observationMode", true }
	});
}

HANDLER_FUNC(vibeguard_session_auto_claim) {
	if (!service_ready(res))
		return;
	const auto token = bearer_token(req);
	if (token.empty()) {
		error_response(res, 401, "invalid_token");
		return;
	}

	ObservationSession session;
	{
		std::lock_guard<std::mutex> lock(sessions_mutex);
		cleanup_expired_locked();
		const auto iterator = sessions.find(token);
		if (iterator == sessions.end()) {
			error_response(res, 401, "session_expired");
			return;
		}
		if (iterator->second.account_id != 0) {
			json_response(res, 200, {
				{ "ok", true }, { "accountBound", true }, { "observationMode", true }
			});
			return;
		}
		session = iterator->second;
	}

	uint32 claimed_account_id = 0;
	for (const auto candidate : recent_accounts_for_address(req.remote_addr, session.opened_unix)) {
		if (auto_claim_account(session, candidate)) {
			claimed_account_id = candidate;
			break;
		}
	}

	if (claimed_account_id != 0) {
		std::lock_guard<std::mutex> lock(sessions_mutex);
		const auto iterator = sessions.find(token);
		if (iterator != sessions.end())
			iterator->second.account_id = claimed_account_id;
		write_observation("auto_claimed", session);
	}

	json_response(res, 200, {
		{ "ok", true },
		{ "accountBound", claimed_account_id != 0 },
		{ "observationMode", true }
	});
}

HANDLER_FUNC(vibeguard_session_close) {
	if (!service_ready(res))
		return;
	const auto token = bearer_token(req);
	if (token.empty()) {
		error_response(res, 401, "invalid_token");
		return;
	}

	ObservationSession closed_session;
	{
		std::lock_guard<std::mutex> lock(sessions_mutex);
		cleanup_expired_locked();
		const auto iterator = sessions.find(token);
		if (iterator == sessions.end()) {
			error_response(res, 401, "session_expired");
			return;
		}
		closed_session = iterator->second;
		sessions.erase(iterator);
	}
	persist_state(closed_session, "closed");
	write_observation("closed", closed_session);
	json_response(res, 200, {
		{ "ok", true },
		{ "observationMode", true }
	});
}
