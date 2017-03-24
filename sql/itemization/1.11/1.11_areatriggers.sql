
-- Disable unavailable instances
-- ATTENTION! Requires update once events are implemented!

UPDATE `areatrigger_teleport` SET `required_level` = 61 WHERE `target_map` = 533;    -- Naxxramas 1.12

-- Enable available instances
-- ATTENTION! Requires update once events are implemented!

UPDATE `areatrigger_teleport` SET `required_level` = 45 WHERE `target_map` = 429;    -- Dire Maul 1.3
UPDATE `areatrigger_teleport` SET `required_level` = 60 WHERE `target_map` = 469;    -- Blackwing Lair 1.6
UPDATE `areatrigger_teleport` SET `required_level` = 60 WHERE `target_map` = 309;    -- Zul'Gurub 1.7
UPDATE `areatrigger_teleport` SET `required_level` = 60 WHERE `target_map` = 531;    -- AQ 40 1.9
UPDATE `areatrigger_teleport` SET `required_level` = 60 WHERE `target_map` = 509;    -- AQ 20 1.9

-- Enable Alterac Valley (1.05)

UPDATE `battleground_template` SET `MinLvl` = 51, `MaxLvl` = 60 WHERE `id` = 1;
