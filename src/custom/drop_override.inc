ACMD_FUNC(npc_set_drop_override)
{
    // Args do script: item_id, rate
    int item_id = script_getnum(st);  // w1
    int rate    = script_getnum(st);  // w2
    int mapid   = map->id;            // mapa atual do player

    struct map_data* mapdata = map_getmapdata(mapid);
    if (!mapdata) return;

    mapdata->drop_override.item_id = item_id;
    mapdata->drop_override.rate = rate;
    mapdata->setMapFlag(MF_DROPOVERRIDE, 1);
}
