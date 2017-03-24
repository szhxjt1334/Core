
-- NPCs to hide: later patch quest givers: 

DELETE FROM `creature` WHERE `id` IN (15189, 15190, 15181, 15183, 15270, 15194, 15070, 14921);

-- No spawn of Azuregos & Kazzak (1.03)

UPDATE `creature` SET `spawnFlags` = 2 WHERE id IN (6109, 12397);



