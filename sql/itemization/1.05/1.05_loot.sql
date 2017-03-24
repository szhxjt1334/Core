-- Patch 1.5 World DB Itemization Changes.
-- To be applied on QA first for testing.

-- Malekis FootWrap Add To Loot Table

SET @refTable = -1;

SELECT * FROM creature_loot_template WHERE entry = 10438;
SELECT @refTable := -mincountOrRef FROM creature_loot_template WHERE entry = 10438 and mincountOrRef < 0 and groupid = 1;

INSERT INTO reference_loot_template VALUES(@refTable,18735,0,1,1,1,0);

-- Onslaught Gridle Add To Loot Table

SET @refTable = -1;

SELECT * FROM creature_loot_template WHERE entry = 11502;
SELECT @refTable := -mincountOrRef FROM creature_loot_template WHERE entry = 11502 and mincountOrRef < 0 and groupid = 10;

INSERT INTO reference_loot_template VALUES(@refTable,19137,0,10,1,1,0);

-- Crystal Adorned Crown Add To Loot Table

SET @refTable = -1;

SELECT * FROM creature_loot_template WHERE entry = 6109;
SELECT @refTable := -mincountOrRef FROM creature_loot_template WHERE entry = 6109 and mincountOrRef < 0 and groupid = 10;

INSERT INTO reference_loot_template VALUES(@refTable,19132,0,10,1,1,0);

-- Animated Chain Necklace Add To Loot Table

SET @refTable = -1;

SELECT * FROM creature_loot_template WHERE entry = 10439;
SELECT @refTable := -mincountOrRef FROM creature_loot_template WHERE entry = 10439 and mincountOrRef < 0 and groupid = 2;

INSERT INTO reference_loot_template VALUES(@refTable,18723,0,2,1,1,0);


-- Nexus Crystal #20725 to Large Brilliant Shard #14344 DB_world Replacement

DELETE FROM disenchant_loot_template WHERE entry = 49 AND item = 20725;
UPDATE disenchant_loot_template SET item = 14344 WHERE entry IN (65, 64);

-- Random Loot Changes

DELETE FROM `creature_loot_template` WHERE `entry` = 2919 AND `item` IN (3771, 3864, 3914, 4552, 4554, 4637, 4784, 7067, 7075, 7848, 7909, 7910, 7912, 7992, 8029, 8150, 10312, 307929, 307930, 307931, 307932, 307933);

-- Golemagg Loot Drop Rates & Faction Drop
update reference_loot_template set ChanceOrQuestChance = 20 where entry = 326158 and item in (18842, 17103, 17072);
update reference_loot_template set condition_id = 3 where entry = 326158 and item = 18824;
update reference_loot_template set condition_id = 2 where entry = 326158 and item = 18829;

-- Pre 1.6-BWL Access Fix
update creature_loot_template set ChanceOrQuestChance=0 where item=18987 and entry=9046;
update creature_loot_template set ChanceOrQuestChance=100 where item=330067 and entry=9046;

-- Thunderfury bindings
DELETE FROM creature_loot_template WHERE item IN (18563, 18564);

-- 1.6 Items (recipes, patterns, plans)
DELETE FROM item_template WHERE entry IN (19447, 19446, 19442, 19216, 19217, 19328, 19329, 19203, 19205, 19204, 19202, 19215, 19327, 19326, 19218, 19445, 22392, 19449, 19448, 19444, 19331, 19332, 19220, 19219, 19330, 19333, 19208, 19211, 19209, 19207, 19206, 19210, 19212, 20761);

-- Disable loot:
update creature_loot_template set ChanceOrQuestChance=0 where item=20424;

-- LBRS Gems Pre-1.11
update creature_loot_template set ChanceOrQuestChance=1 where item=12219;
update creature_loot_template set ChanceOrQuestChance=20 where item=12335;
update creature_loot_template set ChanceOrQuestChance=15 where item=12336;
update creature_loot_template set ChanceOrQuestChance=25 where item=12337;

update creature_loot_template set condition_id=70 where item in (22527,22529,22528,22525,22526);

-- Removing Idols, Totems & Librams From Loot Table

DELETE FROM `creature_loot_template` WHERE `item` IN (23198, 22397, 22398, 23197, 23201, 23203, 22401, 22400, 22395, 22345, 23200, 23199, 22891, 22890, 22891, 22306, 22311, 22313, 22302, 22304, 22305, 22303, 22301, 22722, 22721, 22714, 22204, 22257, 23320, 22720, 22718, 22711, 22712, 22715, 22713);
DELETE FROM `reference_loot_template` WHERE `item` IN (23198, 22397, 22398, 23197, 23201, 23203, 22401, 22400, 22395, 22345, 23200, 23199, 22891, 22890, 22891, 22311, 22313, 22302, 22304, 22305, 22303, 22301, 22722, 22721, 22714, 22204, 22257, 23320, 22720, 22718, 22711, 22712, 22715, 22716, 22713);

-- Removing In Game Items That Were Released After Patch 1.9

DELETE FROM `creature_loot_template` WHERE `item` IN (24102, 23320, 24101, 22890, 22384, 22390, 22390, 22384, 22406, 22403, 22405, 22407, 22404, 22234, 22412, 22049, 22050, 22051, 22052, 22056, 22389, 22329, 22433, 21547, 22402,22408, 22409, 22410, 22411, 22242, 22241, 22240, 22275, 22232, 22231, 22223, 22212, 22208, 22207, 22205, 22204, 22256, 22255, 22254, 22394, 22311, 22305, 22313, 22321, 22225, 22269, 22268, 22267, 22266, 22271, 22253, 22306, 22311, 22313, 22302, 22304, 22305, 22303, 22301);
DELETE FROM `reference_loot_template` WHERE `item` IN (24102, 23320, 24101, 22890, 22384, 22390, 22390, 22384, 22406, 22403, 22405, 22407, 22404, 22234, 22412, 22049, 22050, 22051, 22052, 22056, 22389, 22329, 22433, 21547, 22402,22408, 22409, 22410, 22411, 22242, 22241, 22240, 22275, 22232, 22231, 22223, 22212, 22208, 22207, 22205, 22204, 22256, 22255, 22254, 22394, 22311, 22305, 22313, 22321, 22225, 22269, 22268, 22267, 22266, 22271, 22253, 22306, 22311, 22313, 22302, 22304, 22305, 22303, 22301);

-- Removing ZG 1.11 Drop

DELETE FROM `creature_loot_template` WHERE `item` IN (22722, 22721, 22720, 22718, 22711, 22712, 22715, 22714, 22716, 22713);
DELETE FROM `reference_loot_template` WHERE `item` IN (22722, 22721, 22720, 22718, 22711, 22712, 22715, 22714, 22716, 22713);

-- Removing 1.11 Relics From Drop

DELETE FROM `creature_loot_template` WHERE `item` IN (23198, 22397, 22398, 23197, 23201, 23203, 22401, 22400, 22395, 22345, 23200, 23199);
DELETE FROM `reference_loot_template` WHERE `item` IN (23198, 22397, 22398, 23197, 23201, 23203, 22401, 22400, 22395, 22345, 23200, 23199);

-- GO "Sothos and Jarien's Heirlooms" Has Been Removed

DELETE FROM `gameobject` WHERE `id` = 181083;

-- Setting "Dreamscale Breastplate" Loot Type to BoP (should be shanged back in 1.10)

UPDATE `item_template` SET `bonding` = '1' WHERE `entry` = '20380';
