-- Safe migration for vending_transactions.coin_type
-- Old format: tinyint (0=Zeny, 1=Rops, 2=RMT)
-- New format: enum('Zeny','Rops','RMT')

ALTER TABLE `vending_transactions`
  ADD COLUMN `coin_type_new` enum('Zeny','Rops','RMT') NOT NULL DEFAULT 'Zeny' AFTER `amount`;

UPDATE `vending_transactions`
SET `coin_type_new` = CASE
  WHEN CAST(`coin_type` AS CHAR) IN ('0', '') THEN 'Zeny'
  WHEN CAST(`coin_type` AS CHAR) = '1' THEN 'Rops'
  WHEN CAST(`coin_type` AS CHAR) = '2' THEN 'RMT'
  WHEN CAST(`coin_type` AS CHAR) = 'Zeny' THEN 'Zeny'
  WHEN CAST(`coin_type` AS CHAR) = 'Rops' THEN 'Rops'
  WHEN CAST(`coin_type` AS CHAR) = 'RMT' THEN 'RMT'
  ELSE 'Zeny'
END;

ALTER TABLE `vending_transactions`
  DROP COLUMN `coin_type`;

ALTER TABLE `vending_transactions`
  CHANGE COLUMN `coin_type_new` `coin_type` enum('Zeny','Rops','RMT') NOT NULL DEFAULT 'Zeny';
