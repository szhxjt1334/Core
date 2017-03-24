
DELETE FROM `creature` WHERE `id` = 15306;

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

-- Removing Zandalar Repeatable Quests

DELETE FROM `creature` WHERE `id` IN (15183, 15070, 15306);

-- Spawn of Azuregos, Kazzak 1.3

UPDATE `creature` SET `spawnFlags` = 0 WHERE id IN (6109, 12397);


-- Pre 1.6-BWL Access Fix

update creature set spawntimesecs=108000 where id=9046;

-- Argent Dawn @ Pre-1.11

delete from game_event_creature where event=155;
insert into game_event_creature value (54765,155);
insert into game_event_creature value (54163,155);
insert into game_event_creature value (54120,155);
insert into game_event_creature value (54190,155);
insert into game_event_creature value (54807,155);
insert into game_event_creature value (54766,155);
insert into game_event_creature value (54192,155);
insert into game_event_creature value (54193,155);
insert into game_event_creature value (54805,155);
insert into game_event_creature value (54768,155);
insert into game_event_creature value (54164,155);
insert into game_event_creature value (54186,155);
insert into game_event_creature value (54145,155);
insert into game_event_creature value (52674,155);
insert into game_event_creature value (52675,155);
insert into game_event_creature value (52676,155);
