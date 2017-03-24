
-- Disable unavailable instances

UPDATE `areatrigger_teleport` SET `required_level` = 61 WHERE `target_map` IN 
(
533,    -- Naxxramas 1.12
531,    -- AQ40 (Temple of AQ) 1.9
509,    -- AQ20 (Ruins of AQ) 1.9
429,    -- Dire Maul 1.3
309,    -- Zul'Gurub 1.7
469     -- Blackwing Lair 1.6
);

-- Disable Alterac Valley (1.05)

UPDATE `battleground_template` SET `MinLvl` = 61, `MaxLvl` = 61 WHERE `id` = 1;

-- Remove unsing triggers from DB

DELETE FROM areatrigger_involvedrelation WHERE quest NOT IN (SELECT entry FROM quest_template);
DELETE FROM creature_involvedrelation WHERE quest NOT IN (SELECT entry FROM quest_template);
DELETE FROM creature_questrelation WHERE quest NOT IN (SELECT entry FROM quest_template);
DELETE FROM gameobject_involvedrelation WHERE quest NOT IN (SELECT entry FROM quest_template);
DELETE FROM gameobject_questrelation WHERE quest NOT IN (SELECT entry FROM quest_template);
