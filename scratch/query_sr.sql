
SELECT id, nameid, refine, attribute, card0, card1, card2, card3, equip FROM inventory WHERE char_id = 150057 AND equip > 0 ORDER BY equip ASC;
SELECT * FROM char_reg_num WHERE char_id = 150057;
SELECT char_id, name, class, base_level, job_level, str, agi, vit, `int`, dex, luk, max_hp, max_sp FROM `char` WHERE char_id = 150057;
