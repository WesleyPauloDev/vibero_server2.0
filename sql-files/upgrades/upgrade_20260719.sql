CREATE TABLE IF NOT EXISTS `buyingstore_transactions` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `buyingstore_id` int(10) unsigned NOT NULL DEFAULT '0',
  `buyer_account_id` int(11) unsigned NOT NULL DEFAULT '0',
  `buyer_char_id` int(10) unsigned NOT NULL DEFAULT '0',
  `buyer_name` varchar(30) NOT NULL DEFAULT '',
  `seller_account_id` int(11) unsigned NOT NULL DEFAULT '0',
  `seller_char_id` int(10) unsigned NOT NULL DEFAULT '0',
  `seller_name` varchar(30) NOT NULL DEFAULT '',
  `item_id` int(10) unsigned NOT NULL DEFAULT '0',
  `amount` int(10) unsigned NOT NULL DEFAULT '0',
  `unit_price` bigint(20) unsigned NOT NULL DEFAULT '0',
  `total_price` bigint(20) unsigned NOT NULL DEFAULT '0',
  `purchased_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `idx_buyer_char_id` (`buyer_char_id`),
  KEY `idx_seller_char_id` (`seller_char_id`),
  KEY `idx_item_id` (`item_id`),
  KEY `idx_purchased_at` (`purchased_at`)
) ENGINE=MyISAM;

CREATE TABLE IF NOT EXISTS `trade_transactions` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `party_a_account_id` int(11) unsigned NOT NULL DEFAULT '0',
  `party_a_char_id` int(10) unsigned NOT NULL DEFAULT '0',
  `party_a_name` varchar(30) NOT NULL DEFAULT '',
  `party_b_account_id` int(11) unsigned NOT NULL DEFAULT '0',
  `party_b_char_id` int(10) unsigned NOT NULL DEFAULT '0',
  `party_b_name` varchar(30) NOT NULL DEFAULT '',
  `zeny_a_to_b` bigint(20) unsigned NOT NULL DEFAULT '0',
  `zeny_b_to_a` bigint(20) unsigned NOT NULL DEFAULT '0',
  `completed_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `idx_party_a_account_id` (`party_a_account_id`),
  KEY `idx_party_b_account_id` (`party_b_account_id`),
  KEY `idx_completed_at` (`completed_at`)
) ENGINE=MyISAM;

CREATE TABLE IF NOT EXISTS `trade_transaction_items` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `transaction_id` bigint(20) unsigned NOT NULL,
  `sender_account_id` int(11) unsigned NOT NULL DEFAULT '0',
  `sender_char_id` int(10) unsigned NOT NULL DEFAULT '0',
  `receiver_account_id` int(11) unsigned NOT NULL DEFAULT '0',
  `receiver_char_id` int(10) unsigned NOT NULL DEFAULT '0',
  `item_id` int(10) unsigned NOT NULL DEFAULT '0',
  `amount` int(10) unsigned NOT NULL DEFAULT '0',
  `refine` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `unique_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `idx_transaction_id` (`transaction_id`),
  KEY `idx_sender_account_id` (`sender_account_id`),
  KEY `idx_receiver_account_id` (`receiver_account_id`),
  KEY `idx_item_id` (`item_id`)
) ENGINE=MyISAM;

CREATE TABLE IF NOT EXISTS `rodex_transactions` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `mail_id` bigint(20) unsigned NOT NULL,
  `sender_account_id` int(11) unsigned NOT NULL DEFAULT '0',
  `sender_char_id` int(10) unsigned NOT NULL DEFAULT '0',
  `sender_name` varchar(30) NOT NULL DEFAULT '',
  `receiver_account_id` int(11) unsigned NOT NULL DEFAULT '0',
  `receiver_char_id` int(10) unsigned NOT NULL DEFAULT '0',
  `receiver_name` varchar(30) NOT NULL DEFAULT '',
  `zeny` bigint(20) unsigned NOT NULL DEFAULT '0',
  `sent_at` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_mail_id` (`mail_id`),
  KEY `idx_sender_account_id` (`sender_account_id`),
  KEY `idx_receiver_account_id` (`receiver_account_id`),
  KEY `idx_sent_at` (`sent_at`)
) ENGINE=MyISAM;

CREATE TABLE IF NOT EXISTS `rodex_transaction_items` (
  `id` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
  `mail_id` bigint(20) unsigned NOT NULL,
  `item_index` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `item_id` int(10) unsigned NOT NULL DEFAULT '0',
  `amount` int(10) unsigned NOT NULL DEFAULT '0',
  `refine` tinyint(3) unsigned NOT NULL DEFAULT '0',
  `unique_id` bigint(20) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_mail_item` (`mail_id`,`item_index`),
  KEY `idx_item_id` (`item_id`)
) ENGINE=MyISAM;
