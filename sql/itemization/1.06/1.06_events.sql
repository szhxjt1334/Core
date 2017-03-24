-- Patch 1.6 World DB Itemization Changes.
-- To be applied on QA first for testing.

-- Disable unavailable game events
UPDATE `game_event` SET `disabled` = 1 WHERE `entry` IN
(
14, 15, 40,			-- Stranglethorn Fishing Extravaganza 1.7
66,					-- Dragons of Nightmare 1.8
154,		-- Silithus @ 1.9 (unchecked)
155,		-- Argent dawn @ 1.11 (unchecked)
158,		-- 1.10 Patch : Recipes (unchecked)
160,		-- 1.10 Patch : t0.5 Quest Chain (unchecked)
164,		-- Patch 1.7
165,		-- Patch 1.8
166,		-- Patch 1.9
167,		-- Patch 1.10
168 		-- Patch 1.11 ? 1.12
);

-- Enable available game events
UPDATE `game_event` SET `disabled` = 0 WHERE `entry` IN
(
4, 5, 100, 101, 	-- DarkMoon Faire 1.6
13,					-- Elemental Invasion 1.5
16, 38, 39,			-- Gurubashi Arena 1.5
18,					-- Call to arms: Alterac Valley 1.5
156, 159,			-- Dire Maul extra content(unchecked) 1.3
35, 36, 37,
42, 43, 44,			-- The Maul 1.3
161,				-- Patch 1.3
162,				-- Patch 1.4 | 1.5
163					-- Patch 1.6
);

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
