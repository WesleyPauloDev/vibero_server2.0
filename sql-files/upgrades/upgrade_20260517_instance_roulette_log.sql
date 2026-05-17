-- Apply this on the log database configured as log_db_db (currently log2025).
CREATE TABLE IF NOT EXISTS `instance_roulette_log` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
  `account_id` INT UNSIGNED NOT NULL DEFAULT 0,
  `char_id` INT UNSIGNED NOT NULL DEFAULT 0,
  `char_name` VARCHAR(30) NOT NULL DEFAULT '',
  `event_type` VARCHAR(12) NOT NULL DEFAULT '',
  `category` VARCHAR(10) NOT NULL DEFAULT '',
  `instance_name` VARCHAR(80) NOT NULL DEFAULT '',
  `period_key` VARCHAR(10) NOT NULL DEFAULT '',
  `reward_item_id` INT UNSIGNED NOT NULL DEFAULT 0,
  `reward_item_name` VARCHAR(50) NOT NULL DEFAULT '',
  `reward_amount` INT UNSIGNED NOT NULL DEFAULT 0,
  `guaranteed_item_id` INT UNSIGNED NOT NULL DEFAULT 0,
  `guaranteed_item_name` VARCHAR(50) NOT NULL DEFAULT '',
  `guaranteed_amount` INT UNSIGNED NOT NULL DEFAULT 0,
  `rare_item_id` INT UNSIGNED NOT NULL DEFAULT 0,
  `rare_item_name` VARCHAR(50) NOT NULL DEFAULT '',
  `rare_amount` INT UNSIGNED NOT NULL DEFAULT 0,
  `map` VARCHAR(24) NOT NULL DEFAULT '',
  `created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `idx_created_at` (`created_at`),
  KEY `idx_event_category_period` (`event_type`, `category`, `period_key`),
  KEY `idx_char_id` (`char_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- If this table was created by the previous version, run this once to allow
-- multiple roll/reroll records per day:
-- ALTER TABLE `instance_roulette_log` DROP INDEX `uq_instance_roulette_once`;
