// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom Atcommands
 * Place the body of custom atcommands in this file.
 * Format:
 *	ACMD_FUNC(command_name)
 *	{
 *		<code>;
 *	}
 **/

#include <cctype>

#include <map/vibeguard_observer.hpp>

ACMD_FUNC(guard)
{
	char code[13] = {};
	char extra[2] = {};
	if (message == nullptr || sscanf(message, "%12s %1s", code, extra) != 1) {
		clif_displaymessage(fd, "Uso: @guard CODIGO (copie o codigo exibido pelo VibeGuard)");
		return -1;
	}
	for (size_t index = 0; index < 12; ++index) {
		if (code[index] == '\0' || std::isxdigit(static_cast<unsigned char>(code[index])) == 0) {
			clif_displaymessage(fd, "Codigo VibeGuard invalido ou incompleto.");
			return -1;
		}
		code[index] = static_cast<char>(std::tolower(static_cast<unsigned char>(code[index])));
	}
	if (vibeguard_bind_session(*sd, code)) {
		clif_messagecolor(sd, color_table[COLOR_LIGHT_GREEN],
			"[VibeGuard] Sessao vinculada com sucesso. Monitoramento observacional ativo.", false, SELF);
		return 0;
	}
	clif_displaymessage(fd, "Nao foi possivel vincular: codigo expirado, ja utilizado ou sessao sem heartbeat.");
	return -1;
}

ACMD_FUNC(guardstatus)
{
	vibeguard_show_status(*sd);
	return 0;
}

ACMD_FUNC(guardtest)
{
	if (pc_get_group_level(sd) < 99) {
		clif_displaymessage(fd, "Comando restrito a administradores.");
		return -1;
	}
	const bool persistent = message != nullptr && atoi(message) >= 2;
	vibeguard_test_warning(*sd, persistent);
	clif_displaymessage(fd, persistent
		? "Teste VibeGuard: aviso persistente enviado."
		: "Teste VibeGuard: aviso inicial enviado.");
	return 0;
}

//ACMD_FUNC(newcommand)
//{
//	clif_displaymessage(fd, "It works!");
//	clif_specialeffect(&sd->bl, EF_HEARTCASTING, AREA);
//	return 0;
//}
