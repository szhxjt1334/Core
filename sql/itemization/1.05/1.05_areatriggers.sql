
-- Enable Alterac Valley 1.5

UPDATE `battleground_template` SET `MinLvl` = 51, `MaxLvl` = 60 WHERE `id` = 1;

-- Enable Available Instances

UPDATE `areatrigger_teleport` SET `required_level` = 45 WHERE `target_map` = 429;    -- Dire Maul 1.3


-- Disable unavailable instances

UPDATE `areatrigger_teleport` SET `required_level` = 61 WHERE `target_map` IN 
(
533,    -- Naxxramas 1.12
531,    -- AQ40 (Temple of AQ) 1.9
509,    -- AQ20 (Ruins of AQ) 1.9
309,    -- Zul'Gurub 1.7
469     -- Blackwing Lair 1.6
);