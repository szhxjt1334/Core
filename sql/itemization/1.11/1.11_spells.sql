
-- Zanza's Potent Potables

DELETE FROM `spell_mod` WHERE `Id` IN (24382, 24417, 24383);
UPDATE `quest_template` SET `ReqItemCount1` = 1 WHERE `entry` = 8243;
DELETE FROM `spell_disabled` WHERE `entry` = 24417;
