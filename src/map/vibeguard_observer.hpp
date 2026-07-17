#ifndef VIBEGUARD_OBSERVER_HPP
#define VIBEGUARD_OBSERVER_HPP

#include <common/cbasetypes.hpp>

class map_session_data;

enum class e_vibeguard_action : uint8 {
	MOVE,
	ATTACK,
	PICKUP,
	SKILL,
};

bool vibeguard_bind_session(map_session_data& sd, const char* pairing_code);
void vibeguard_observe_action(map_session_data& sd, e_vibeguard_action action);
void vibeguard_show_status(map_session_data& sd);
void vibeguard_test_warning(map_session_data& sd, bool persistent);
void vibeguard_observer_init();
void vibeguard_observer_final();

#endif
