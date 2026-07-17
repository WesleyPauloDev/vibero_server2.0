// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom battle config initializations
 * Place the definition of custom battle configuration initializations in this file.
 *
 * Do not forget to add a comma at the end of the line, if you want to add another config
 *
 * Format:
 *	 { "name", &battle_config.<variable name>, <default value>, <minimum value>, <maximum value> },
 **/

{ "vibeguard_enforcement_mode",       &battle_config.vibeguard_enforcement_mode,       0, 0, 2 },
{ "vibeguard_warning_seconds",        &battle_config.vibeguard_warning_seconds,       30, 5, 3600 },
{ "vibeguard_grace_seconds",          &battle_config.vibeguard_grace_seconds,         90, 10, 7200 },
{ "vibeguard_heartbeat_ttl_seconds",  &battle_config.vibeguard_heartbeat_ttl_seconds, 60, 20, 300 },
{ "vibeguard_exempt_group_level",     &battle_config.vibeguard_exempt_group_level,     1, 0, 99 },

