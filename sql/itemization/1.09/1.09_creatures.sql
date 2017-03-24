
-- Spawn of Azuregos, Kazzak (1.03)

UPDATE `creature` SET `spawnFlags` = 0 WHERE id IN (6109, 12397);

-- Changing War Effort NPCs Model Sizes

UPDATE `creature_template` SET `scale` = '1.35' WHERE `entry` IN (15535, 15528, 15532, 15739, 15477);

-- NPC "Number Two" Tweaks

UPDATE `creature_template` set `minhealth` = '128600' , `maxhealth` = '128600' , `armor` = '6091' , `mindmg` = '700' , `maxdmg` = '1100' where `entry` = '15554';

-- NPC "Doctor Weavil" Tweaks

UPDATE `creature_template` set `minhealth` = '1000000' , `maxhealth` = '1000000' , `armor` = '5941' , `attackpower` = '490' where `entry` = '15552';