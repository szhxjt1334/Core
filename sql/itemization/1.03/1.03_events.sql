
-- Disable unavailable game events

UPDATE `game_event` SET `disabled` = 1 WHERE `entry` IN
(
4, 5, 100, 101, 	-- DarkMoon Faire 1.6
13,					-- Elemental Invasion 1.5
16, 38, 39,			-- Gurubashi Arena 1.5
14, 15, 40,			-- Stranglethorn Fishing Extravaganza 1.7
66,					-- Dragons of Nightmare 1.8
18,					-- Call to arms: Alterac Valley 1.5
154,		-- Silithus @ 1.9 (unchecked)
155,		-- Argent dawn @ 1.11 (unchecked)
158,		-- 1.10 Patch : Recipes (unchecked)
160,		-- 1.10 Patch : t0.5 Quest Chain (unchecked)
162,		-- Patch 1.4 | 1.5
163,		-- Patch 1.6
164,		-- Patch 1.7
165,		-- Patch 1.8
166,		-- Patch 1.9
167,		-- Patch 1.10
168 		-- Patch 1.11 ? 1.12
);

-- Enable available game events

UPDATE `game_event` SET `disabled` = 0 WHERE `entry` IN
(
156, 159,			-- Dire Maul extra content(unchecked) 1.3
35, 36, 37,
42, 43, 44,			-- The Maul 1.3
161					-- Patch 1.3
);

-- Disable Gurubashi Chest event (pre-1.04)

update game_event set start_time="2016-08-04 03:00:00" where entry=16;
update game_event set start_time="2016-08-04 06:00:00" where entry=38;
update game_event set start_time="2016-08-04 05:55:00" where entry=39;

-- Cloth turning NPC (reput ... => mount)

delete from game_event where entry=159;
replace into game_event value (159, "2025-03-30 00:00:00", "2030-03-30 00:00:00", 1, 2, 0, "DM Release : Cloth turning NPC", 0);
delete from game_event_creature where event=159;
replace into game_event_creature value (79821,159);
replace into game_event_creature value (1819,159);
replace into game_event_creature value (37,159);
replace into game_event_creature value (46559,159);
replace into game_event_creature value (3421,159);
replace into game_event_creature value (3422,159);
replace into game_event_creature value (24867,159);
replace into game_event_creature value (31903,159);

-- Argent Dawn @ Pre-1.11

delete from game_event where entry=155;
insert into game_event value (155, "2025-03-30 00:00:00", "2030-03-30 00:00:00", 1, 2, 0, "Argent dawn @ 1.11", 0);
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
