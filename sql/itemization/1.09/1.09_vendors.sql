
-- Removing Items From NPC "Illiyana Moonblaze"

DELETE FROM `npc_vendor` WHERE `entry` = 14753 AND `item` IN (22748, 22753, 22749, 22750, 22672, 22752);

-- Removing Items From NPC "Kelm Hargunth"

DELETE FROM `npc_vendor` WHERE `entry` = 14754 AND `item` IN (22673, 22740, 22741, 22676, 22651, 22747);

-- Adding Tailoring Recipes to NPC "Mishta" in Cenarion Hold

REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','2320','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','2321','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','2325','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','2604','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','2678','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','2692','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','2880','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','2901','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','2928','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','3371','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','3372','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','3466','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','3713','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','3857','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','4289','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','4291','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','4340','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','4341','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','4342','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','4399','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','4400','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','5956','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','6217','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','6256','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','6530','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','6532','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','7005','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','8343','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','8925','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','14341','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','18256','0','0');
REPLACE `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15179','22310','0','0');

-- New Recipes to NPC "Kania"

DELETE FROM `npc_vendor` WHERE `entry` = 15419 AND `item` IN (20732, 20733);

INSERT INTO `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15419','20732','0','0');
INSERT INTO `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`) VALUES ('15419','20733','0','0');