// Copyright (c) rAthena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

/**
 * Custom Script Commands
 * Place the body of custom script commands in this file.
 * Format:
 *	BUILDIN_FUNC(command_name)
 *	{
 *		<code>;
 *	}
 **/

#include <unordered_set>

BUILDIN_FUNC(is_maxbaselv)
{
	map_session_data* sd;
	if (!script_rid2sd(sd)) {
		script_pushint(st, 0);
		return SCRIPT_CMD_SUCCESS;
	}
	script_pushint(st, pc_is_maxbaselv(sd) ? 1 : 0);
	return SCRIPT_CMD_SUCCESS;
}

BUILDIN_FUNC(is_maxjoblv)
{
	map_session_data* sd;
	if (!script_rid2sd(sd)) {
		script_pushint(st, 0);
		return SCRIPT_CMD_SUCCESS;
	}
	script_pushint(st, pc_is_maxjoblv(sd) ? 1 : 0);
	return SCRIPT_CMD_SUCCESS;
}

BUILDIN_FUNC(autostore_loot)
{
	map_session_data* sd;
	if (!script_rid2sd(sd)) {
		script_pushint(st, 0);
		return SCRIPT_CMD_SUCCESS;
	}

	int32 threshold = script_hasdata(st, 2) ? script_getnum(st, 2) : 70;
	int32 max_w = sd->max_weight > 0 ? sd->max_weight : 1;
	int32 weight_pct = (sd->weight * 100) / max_w;
	if (weight_pct < threshold) {
		script_pushint(st, 0);
		return SCRIPT_CMD_SUCCESS;
	}

	if (sd->state.storage_flag == 1 || sd->state.vending || sd->state.buyingstore || sd->state.trading) {
		script_pushint(st, 0);
		return SCRIPT_CMD_SUCCESS;
	}

	std::unordered_set<t_itemid> protected_items;
	protected_items.insert(7621);    // Amuleto de Ziegfried
	protected_items.insert(601);     // Asa de Mosca
	protected_items.insert(602);     // Asa de Borboleta
	protected_items.insert(12213);   // Asa de Mosca Gigante
	protected_items.insert(1001470); // Pergaminho de Tempo Auto

	// Protege itens configurados no @restock
	for (uint32 r = 0; r < RESTOCK_MAX_ITEMS; ++r) {
		t_itemid restock_id = static_cast<t_itemid>(pc_readglobalreg(sd, reference_uid(add_str(RESTOCK_ITEM_VAR), r)));
		if (restock_id > 0)
			protected_items.insert(restock_id);
	}

	// Protege itens configurados no @autopot (le os registros persistentes do char)
	t_itemid autopot_hp = static_cast<t_itemid>(pc_readglobalreg(sd, add_str("autopot_hp_id")));
	if (autopot_hp > 0) protected_items.insert(autopot_hp);
	t_itemid autopot_sp = static_cast<t_itemid>(pc_readglobalreg(sd, add_str("autopot_sp_id")));
	if (autopot_sp > 0) protected_items.insert(autopot_sp);

	// Protege itens extras passados como argumentos: autostore_loot(threshold, item1, item2, ...)
	for (int32 arg = 3; script_hasdata(st, arg); ++arg) {
		t_itemid extra_item = static_cast<t_itemid>(script_getnum(st, arg));
		if (extra_item > 0) {
			protected_items.insert(extra_item);
		}
	}

	bool opened_by_us = false;
	if (sd->state.storage_flag == 0) {
		if (storage_storageopen(sd) == 0) {
			opened_by_us = true;
		} else {
			script_pushint(st, 0);
			return SCRIPT_CMD_SUCCESS;
		}
	}

	int32 stored_count = 0;
	for (int32 i = 0; i < MAX_INVENTORY; ++i) {
		if (sd->inventory.u.items_inventory[i].amount <= 0)
			continue;
		if (sd->inventory.u.items_inventory[i].equip != 0)
			continue;
		if (sd->inventory.u.items_inventory[i].favorite)
			continue;

		t_itemid nameid = sd->inventory.u.items_inventory[i].nameid;
		if (protected_items.find(nameid) != protected_items.end())
			continue;

		if (!itemdb_canstore(&sd->inventory.u.items_inventory[i], pc_get_group_level(sd)))
			continue;

		std::shared_ptr<item_data> data = item_db.find(nameid);
		if (!data)
			continue;

		int32 amount = sd->inventory.u.items_inventory[i].amount;
		storage_storageadd(sd, &sd->storage, i, amount);
		stored_count++;

		int32 cur_weight_pct = (sd->weight * 100) / (sd->max_weight > 0 ? sd->max_weight : 1);
		if (cur_weight_pct < 50)
			break;
	}

	if (opened_by_us) {
		storage_storageclose(sd);
	}

	script_pushint(st, stored_count);
	return SCRIPT_CMD_SUCCESS;
}
