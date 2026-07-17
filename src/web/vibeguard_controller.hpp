// VibeGuard observation-only session endpoints.
#ifndef VIBEGUARD_CONTROLLER_HPP
#define VIBEGUARD_CONTROLLER_HPP

#include "http.hpp"

HANDLER_FUNC(vibeguard_session_open);
HANDLER_FUNC(vibeguard_session_claim);
HANDLER_FUNC(vibeguard_session_auto_claim);
HANDLER_FUNC(vibeguard_session_heartbeat);
HANDLER_FUNC(vibeguard_session_close);
bool vibeguard_storage_initialize();

#endif
