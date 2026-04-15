CREATE TABLE IF NOT EXISTS `instance_monitor_member_state` (
  `account_id` int(11) unsigned NOT NULL,
  `instance_code` varchar(100) NOT NULL,
  `instance_name` varchar(100) NOT NULL,
  `tried_count` int(11) unsigned NOT NULL DEFAULT 0,
  `completed_count` int(11) unsigned NOT NULL DEFAULT 0,
  `last_try_at` datetime DEFAULT NULL,
  `last_completed_at` datetime DEFAULT NULL,
  PRIMARY KEY (`account_id`,`instance_code`),
  KEY `idx_instance_code` (`instance_code`),
  KEY `idx_last_completed_at` (`last_completed_at`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;