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
-- Golemagg

update creature_loot_template set ChanceOrQuestChance=30 where mincountOrRef=-326158;

-- Magmadar

update creature_loot_template set ChanceOrQuestChance=30 where mincountOrRef=-326152;
delete from reference_loot_template where entry = 326150 and item in (16835, 16867, 16847, 16855);
update reference_loot_template set condition_id = 0 where entry = 326150 and item in (16814, 16810, 16796, 16822);
delete from reference_loot_template
where entry = 326151 and item in (16814, 16810, 16796, 16822);
update reference_loot_template set condition_id = 0 where entry = 326151 and item in (16835, 16867, 16847);
replace into reference_loot_template values (326151, 16855, 0, 3, 1, 1, 3);

-- Shazzrah

delete from reference_loot_template where item=17077 and entry in (326586,326299,326276);

-- Majordomo

delete from gameobject_loot_template where item=18703 and entry=16719;

-- Ragnaros

update creature_loot_template set ChanceOrQuestChance=15 where mincountOrRef=-330001;
replace into reference_loot_template value (322538, 17067, 0, 3, 1, 1, 0);
update creature_loot_template set ChanceOrQuestChance=0 where mincountOrRef=-322539;
update creature_loot_template set ChanceOrQuestChance=25 where mincountOrRef=-322538;

-- Garr

update creature_loot_template set ChanceOrQuestChance=30 where mincountOrRef=-326264;

-- Ragnaros

update creature_loot_template set maxcount=1,chanceorquestchance=100 where mincountOrRef=-330001;

-- Onyxia loot table

update creature_loot_template set ChanceOrQuestChance=100 where mincountOrRef=-322538 and entry=10184;

-- Zul'farrak pre-1.04 fix

update creature_loot_template set ChanceOrQuestChance=0 where mincountOrRef=-318124;
replace into creature_loot_template value (7271,319573,100,4,-319573,1,0);

-- Garr Loot Tables

delete from reference_loot_template
where entry = 326262 and item in (16813, 16795, 16808, 16821);
update reference_loot_template set condition_id = 0 where entry = 326262 and item != 16854;
replace into reference_loot_template values (326262, 16842, 0, 2, 1, 1, 2);
delete from reference_loot_template where entry = 326263 and item in (16866, 16834, 16846, 16854);
update reference_loot_template set condition_id = 0 where entry = 326263;

-- Clean-up

delete from creature_loot_template where entry = 12057 and item = 330070;
delete from creature_loot_template where entry = 12057 and item = 330071;
delete from reference_loot_template where entry = 330070;
delete from reference_loot_template where entry = 330071;
delete from creature_loot_template where entry = 11982 and item = 330009;
delete from creature_loot_template where entry = 11982 and item = 330010;
delete from reference_loot_template where entry = 330009;
delete from reference_loot_template where entry = 330010;

-- Pre 1.6-BWL Access Fix

update creature_loot_template set ChanceOrQuestChance=0 where item=18987 and entry=9046;
update creature_loot_template set ChanceOrQuestChance=100 where item=330067 and entry=9046;

-- Thunderfury Bindings

DELETE FROM creature_loot_template WHERE item IN (18563, 18564);

-- Disable loot:

update creature_loot_template set ChanceOrQuestChance=0 where item=20424;

-- LBRS Gems Pre-1.11

update creature_loot_template set ChanceOrQuestChance=1 where item=12219;
update creature_loot_template set ChanceOrQuestChance=20 where item=12335;
update creature_loot_template set ChanceOrQuestChance=15 where item=12336;
update creature_loot_template set ChanceOrQuestChance=25 where item=12337;

-- Argent Dawn item

update creature_loot_template set condition_id=70 where item in (22527,22529,22528,22525,22526);

-- Removing Idols, Totems & Librams From Loot Table

DELETE FROM `creature_loot_template` WHERE `item` IN (23198, 22397, 22398, 23197, 23201, 23203, 22401, 22400, 22395, 22345, 23200, 23199, 22891, 22890, 22891, 22306, 22311, 22313, 22302, 22304, 22305, 22303, 22301, 22722, 22721, 22714, 22204, 22257, 23320, 22720, 22718, 22711, 22712, 22715, 22713);
DELETE FROM `reference_loot_template` WHERE `item` IN (23198, 22397, 22398, 23197, 23201, 23203, 22401, 22400, 22395, 22345, 23200, 23199, 22891, 22890, 22891, 22311, 22313, 22302, 22304, 22305, 22303, 22301, 22722, 22721, 22714, 22204, 22257, 23320, 22720, 22718, 22711, 22712, 22715, 22716, 22713);

-- Removing In Game Items That Were Released After Patch 1.9

DELETE FROM `creature_loot_template` WHERE `item` IN (24102, 23320, 24101, 22890, 22384, 22390, 22390, 22384, 22406, 22403, 22405, 22407, 22404, 22234, 22412, 22049, 22050, 22051, 22052, 22056, 22389, 22329);
DELETE FROM `reference_loot_template` WHERE `item` IN (24102, 23320, 24101, 22890, 22384, 22390, 22390, 22384, 22406, 22403, 22405, 22407, 22404, 22234, 22412, 22049, 22050, 22051, 22052, 22056, 22389, 22329);

-- Baroness Anastari

update creature_loot_template set ChanceOrQuestChance=0 where item=323242;
replace into reference_loot_template value (323241, 13534, 0, 1, 1, 1, 0);
replace into reference_loot_template value (323241, 16704, 0, 1, 1, 1, 0);

-- Nerub'enkan

update creature_loot_template set ChanceOrQuestChance=0 where item=323244;
replace into reference_loot_template value (323243, 13529, 0, 1, 1, 1, 0);
replace into reference_loot_template value (323243, 16675, 0, 1, 1, 1, 0);

-- Maleki the Pallid

update creature_loot_template set ChanceOrQuestChance=0 where item=323246;
replace into reference_loot_template value (323245, 13524, 0, 1, 1, 1, 0);
replace into reference_loot_template value (323245, 16691, 0, 1, 1, 1, 0);

-- Magistrate Barthilas

update creature_loot_template set item=330068,mincountOrRef=-330068 where entry=10435 and item=323240;
replace into reference_loot_template value (330068, 13376, 10, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 8283, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 8289, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 8290, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 8296, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 8300, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 8307, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 8308, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 8309, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 8310, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10105, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10136, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10137, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10140, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10144, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10146, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10147, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10149, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10154, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10155, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10156, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10161, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10210, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10217, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10219, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10226, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10228, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10230, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10249, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10256, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10258, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10267, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10275, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10363, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10373, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10376, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10379, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10380, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10383, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10387, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 10391, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 11979, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 12016, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 12046, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 12057, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14303, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14306, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14308, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14312, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14317, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14323, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14324, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14325, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14330, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14337, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14458, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14465, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14684, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14688, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14863, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14865, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14868, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14935, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14936, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14966, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14973, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14976, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 14977, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15189, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15192, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15194, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15239, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15266, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15278, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15282, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15288, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15325, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15430, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15433, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15434, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15436, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15437, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15660, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15664, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15665, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15667, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15670, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15672, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15674, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15676, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15677, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15679, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15683, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15931, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15943, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15986, 0, 1, 1, 1, 0);
replace into reference_loot_template value (330068, 15987, 0, 1, 1, 1, 0);