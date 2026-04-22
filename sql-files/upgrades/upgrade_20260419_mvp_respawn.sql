CREATE TABLE IF NOT EXISTS `mvp_respawn` (
  `spawn_key` varchar(16) NOT NULL,
  `mob_id` int(11) unsigned NOT NULL DEFAULT '0',
  `map` varchar(16) NOT NULL DEFAULT '',
  `source_file` varchar(255) NOT NULL DEFAULT '',
  `source_line` int(11) unsigned NOT NULL DEFAULT '0',
  `spawn_idx` smallint(5) unsigned NOT NULL DEFAULT '0',
  `kill_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `respawn_time` bigint(20) unsigned NOT NULL DEFAULT '0',
  `killer_name` varchar(24) NOT NULL DEFAULT '',
  `tomb_x` smallint(5) unsigned NOT NULL DEFAULT '0',
  `tomb_y` smallint(5) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`spawn_key`),
  KEY `idx_respawn_time` (`respawn_time`)
) ENGINE=MyISAM;
