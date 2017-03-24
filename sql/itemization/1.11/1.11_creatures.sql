
-- Enable spawn of Azuregos & Kazzak (1.03)

UPDATE `creature` SET `spawnFlags` = 0 WHERE id IN (6109, 12397);
