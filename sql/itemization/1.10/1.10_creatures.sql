-- Patch 1.10 World DB Itemization Changes. 
-- To be applied on QA first for testing.

-- Zanza's Potent Potables
REPLACE INTO `spell_mod` (`Id`, `AttributesEx3`, `Comment`) VALUES 
(24382, 1048576, 'Spirit of Zanza - persist through death (before 1.12)'),
(24417, 1048576, 'Sheen of Zanza - persist through death (before 1.12)'),
(24383, 1048576, 'Swiftness of Zanza - persist through death (before 1.12)');
DELETE FROM `spell_disabled` WHERE `entry` = 24417;

-- Spawn of Azuregos, Kazzak 1.3
UPDATE `creature` SET `spawnFlags` = 0 WHERE id IN (6109, 12397);

-- disable unavailable game events
UPDATE `game_event` SET `disabled` = 1 WHERE `entry` IN
(
155,		-- Argent dawn @ 1.11 (unchecked)
168 		-- Patch 1.11 ? 1.12
);

-- Enable available game events
UPDATE `game_event` SET `disabled` = 0 WHERE `entry` IN
(
14, 15, 40,			-- Stranglethorn Fishing Extravaganza 1.7
4, 5, 100, 101, 	-- DarkMoon Faire 1.6
13,					-- Elemental Invasion 1.5
16, 38, 39,			-- Gurubashi Arena 1.5
18,					-- Call to arms: Alterac Valley 1.5
156, 159,			-- Dire Maul extra content(unchecked) 1.3
35, 36, 37,
42, 43, 44,			-- The Maul 1.3
66,					-- Dragons of Nightmare 1.8
161,				-- Patch 1.3
162,				-- Patch 1.4 | 1.5
163,				-- Patch 1.6
164,				-- Patch 1.7
165,				-- Patch 1.8
154,				-- Silithus @ 1.9 (unchecked)
166,				-- Patch 1.9
158,				-- 1.10 Patch : Recipes (unchecked)
160,				-- 1.10 Patch : t0.5 Quest Chain (unchecked)
167					-- Patch 1.10
);

-- Enable Alterac Valley 1.5
UPDATE `battleground_template` SET `MinLvl` = 51, `MaxLvl` = 60 WHERE `id` = 1;

-- "Thunderfury" Stats Have Been Corrected (main hand untill 1.12)
UPDATE item_template SET InventoryType = 21 WHERE entry = 19019;

-- Disable unavailable instances
-- ATTENTION! Requires update once events are implemented!
UPDATE `areatrigger_teleport` SET `required_level` = 61 WHERE `target_map` IN 
(
533,    -- Naxxramas 1.12
531,    -- AQ40 (Temple of AQ) 1.9
509	    -- AQ20 (Ruins of AQ) 1.9
);

-- Enable available instances
-- ATTENTION! Requires update once events are implemented!
UPDATE `areatrigger_teleport` SET `required_level` = 45 WHERE `target_map` = 429;    -- Dire Maul 1.3
UPDATE `areatrigger_teleport` SET `required_level` = 60 WHERE `target_map` = 469;    -- Blackwing Lair 1.6
UPDATE `areatrigger_teleport` SET `required_level` = 60 WHERE `target_map` = 309;    -- Zul'Gurub 1.7

-- Transition Patch For Pre-1.4 Mounts
	-- Undead
insert into npc_vendor value (4731,11559,0,0);
	-- Tauren
insert into npc_vendor value (3685,11547,0,0);
insert into npc_vendor value (3685,11548,0,0);
	-- Orc
insert into npc_vendor value (3362,11549,0,0);
insert into npc_vendor value (3362,11550,0,0);
	-- Troll
insert into npc_vendor value (7952,11545,0,0);
insert into npc_vendor value (7952,11546,0,0);
	-- Human
insert into npc_vendor value (384,11551,0,0);
insert into npc_vendor value (4885,11551,0,0);
insert into npc_vendor value (2357,11551,0,0);
insert into npc_vendor value (1460,11551,0,0);
insert into npc_vendor value (384,11552,0,0);
insert into npc_vendor value (4885,11552,0,0);
insert into npc_vendor value (2357,11552,0,0);
insert into npc_vendor value (1460,11552,0,0);
	-- Dwarf
insert into npc_vendor value (1261,11553,0,0);
insert into npc_vendor value (1261,11554,0,0);
	-- Elf
insert into npc_vendor value (4730,11555,0,0);
insert into npc_vendor value (4730,11556,0,0);
	-- Gnome
insert into npc_vendor value (7955,11557,0,0);
insert into npc_vendor value (7955,11558,0,0);
--  -----------------------------------------------------------
--  Insert mounts quests (transforms pre 1.4 mount to post 1.4)
--  -----------------------------------------------------------
insert into creature_questrelation value    (3685,7663);
insert into creature_involvedrelation value (3685,7663);
insert into creature_questrelation value    (3685,7662);
insert into creature_involvedrelation value (3685,7662);
insert into creature_questrelation value    (3362,7660);
insert into creature_involvedrelation value (3362,7660);
insert into creature_questrelation value    (3362,7661);
insert into creature_involvedrelation value (3362,7661);
insert into creature_questrelation value    (7952,7664);
insert into creature_involvedrelation value (7952,7664);
insert into creature_questrelation value    (7952,7665);
insert into creature_involvedrelation value (7952,7665);
insert into creature_questrelation value    ( 384,7677);
insert into creature_involvedrelation value ( 384,7677);
insert into creature_questrelation value    ( 384,7678);
insert into creature_involvedrelation value ( 384,7678);
insert into creature_questrelation value    (4885,7677);
insert into creature_involvedrelation value (4885,7677);
insert into creature_questrelation value    (4885,7678);
insert into creature_involvedrelation value (4885,7678);
insert into creature_questrelation value    (2357,7677);
insert into creature_involvedrelation value (2357,7677);
insert into creature_questrelation value    (2357,7678);
insert into creature_involvedrelation value (2357,7678);
insert into creature_questrelation value    (1460,7677);
insert into creature_involvedrelation value (1460,7677);
insert into creature_questrelation value    (1460,7678);
insert into creature_involvedrelation value (1460,7678);
insert into creature_questrelation value    (1261,7673);
insert into creature_involvedrelation value (1261,7673);
insert into creature_questrelation value    (1261,7674);
insert into creature_involvedrelation value (1261,7674);
insert into creature_questrelation value    (4730,7671);
insert into creature_involvedrelation value (4730,7671);
insert into creature_questrelation value    (4730,7672);
insert into creature_involvedrelation value (4730,7672);
insert into creature_questrelation value    (7955,7675);
insert into creature_involvedrelation value (7955,7675);
insert into creature_questrelation value    (7955,7676);
insert into creature_involvedrelation value (7955,7676);

-- Golemagg Loot Drop Rates & Faction Drop
update reference_loot_template set ChanceOrQuestChance = 20 where entry = 326158 and item in (18842, 17103, 17072);
update reference_loot_template set condition_id = 3 where entry = 326158 and item = 18824;
update reference_loot_template set condition_id = 2 where entry = 326158 and item = 18829;

-- LBRS Gems Pre-1.11
update creature_loot_template set ChanceOrQuestChance=1 where item=12219;
update creature_loot_template set ChanceOrQuestChance=20 where item=12335;
update creature_loot_template set ChanceOrQuestChance=15 where item=12336;
update creature_loot_template set ChanceOrQuestChance=25 where item=12337;

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
-- Argent Dawn item
delete from conditions where condition_entry=70;
insert into conditions value (70,12,155,0);
update creature_loot_template set condition_id=70 where item in (22527,22529,22528,22525,22526);

-- Old PvP set Patch Vendor Pre-1.11
-- Delete 1.11 PvP Items
delete from item_template where entry in (22843,22852,22855,22856,22857,22858,22859,22860,22862,22863,22864,22865,22867,22868,22869,22870,22872,22873,22874,22875,22876,22877,22878,22879,22880,22881,22882,22883,22884,22885,22886,22887,23243,23244,23251,23252,23253,23254,23255,23256,23257,23258,23259,23260,23261,23262,23263,23264,23272,23273,23274,23275,23276,23277,23278,23279,23280,23281,23282,23283,23284,23285,23286,23287,23288,23289,23290,23291,23292,23293,23294,23295,23296,23297,23298,23299,23300,23301,23302,23303,23304,23305,23306,23307,23308,23309,23310,23311,23312,23313,23314,23315,23316,23317,23318,23319,23451,23452,23453,23454,23455,23456,23464,23465,23466,23467,23468,23469);
-- Vendor Pre 1.11 PvP items
delete from npc_vendor where entry in (12785,12795);
insert into npc_vendor value (12785, 16369, 0, 0);
insert into npc_vendor value (12785, 16391, 0, 0);
insert into npc_vendor value (12785, 16392, 0, 0);
insert into npc_vendor value (12785, 16393, 0, 0);
insert into npc_vendor value (12785, 16396, 0, 0);
insert into npc_vendor value (12785, 16397, 0, 0);
insert into npc_vendor value (12785, 16401, 0, 0);
insert into npc_vendor value (12785, 16403, 0, 0);
insert into npc_vendor value (12785, 16405, 0, 0);
insert into npc_vendor value (12785, 16406, 0, 0);
insert into npc_vendor value (12785, 16409, 0, 0);
insert into npc_vendor value (12785, 16410, 0, 0);
insert into npc_vendor value (12785, 16413, 0, 0);
insert into npc_vendor value (12785, 16414, 0, 0);
insert into npc_vendor value (12785, 16415, 0, 0);
insert into npc_vendor value (12785, 16416, 0, 0);
insert into npc_vendor value (12785, 16417, 0, 0);
insert into npc_vendor value (12785, 16418, 0, 0);
insert into npc_vendor value (12785, 16419, 0, 0);
insert into npc_vendor value (12785, 16420, 0, 0);
insert into npc_vendor value (12785, 16421, 0, 0);
insert into npc_vendor value (12785, 16422, 0, 0);
insert into npc_vendor value (12785, 16423, 0, 0);
insert into npc_vendor value (12785, 16424, 0, 0);
insert into npc_vendor value (12785, 16425, 0, 0);
insert into npc_vendor value (12785, 16426, 0, 0);
insert into npc_vendor value (12785, 16427, 0, 0);
insert into npc_vendor value (12785, 16428, 0, 0);
insert into npc_vendor value (12785, 16429, 0, 0);
insert into npc_vendor value (12785, 16430, 0, 0);
insert into npc_vendor value (12785, 16431, 0, 0);
insert into npc_vendor value (12785, 16432, 0, 0);
insert into npc_vendor value (12785, 16433, 0, 0);
insert into npc_vendor value (12785, 16434, 0, 0);
insert into npc_vendor value (12785, 16435, 0, 0);
insert into npc_vendor value (12785, 16436, 0, 0);
insert into npc_vendor value (12785, 16437, 0, 0);
insert into npc_vendor value (12785, 16440, 0, 0);
insert into npc_vendor value (12785, 16441, 0, 0);
insert into npc_vendor value (12785, 16442, 0, 0);
insert into npc_vendor value (12785, 16443, 0, 0);
insert into npc_vendor value (12785, 16444, 0, 0);
insert into npc_vendor value (12785, 16446, 0, 0);
insert into npc_vendor value (12785, 16448, 0, 0);
insert into npc_vendor value (12785, 16449, 0, 0);
insert into npc_vendor value (12785, 16450, 0, 0);
insert into npc_vendor value (12785, 16451, 0, 0);
insert into npc_vendor value (12785, 16452, 0, 0);
insert into npc_vendor value (12785, 16453, 0, 0);
insert into npc_vendor value (12785, 16454, 0, 0);
insert into npc_vendor value (12785, 16455, 0, 0);
insert into npc_vendor value (12785, 16456, 0, 0);
insert into npc_vendor value (12785, 16457, 0, 0);
insert into npc_vendor value (12785, 16459, 0, 0);
insert into npc_vendor value (12785, 16462, 0, 0);
insert into npc_vendor value (12785, 16463, 0, 0);
insert into npc_vendor value (12785, 16465, 0, 0);
insert into npc_vendor value (12785, 16466, 0, 0);
insert into npc_vendor value (12785, 16467, 0, 0);
insert into npc_vendor value (12785, 16468, 0, 0);
insert into npc_vendor value (12785, 16471, 0, 0);
insert into npc_vendor value (12785, 16472, 0, 0);
insert into npc_vendor value (12785, 16473, 0, 0);
insert into npc_vendor value (12785, 16474, 0, 0);
insert into npc_vendor value (12785, 16475, 0, 0);
insert into npc_vendor value (12785, 16476, 0, 0);
insert into npc_vendor value (12785, 16477, 0, 0);
insert into npc_vendor value (12785, 16478, 0, 0);
insert into npc_vendor value (12785, 16479, 0, 0);
insert into npc_vendor value (12785, 16480, 0, 0);
insert into npc_vendor value (12785, 16483, 0, 0);
insert into npc_vendor value (12785, 16484, 0, 0);
insert into npc_vendor value (12785, 17562, 0, 0);
insert into npc_vendor value (12785, 17564, 0, 0);
insert into npc_vendor value (12785, 17566, 0, 0);
insert into npc_vendor value (12785, 17567, 0, 0);
insert into npc_vendor value (12785, 17568, 0, 0);
insert into npc_vendor value (12785, 17569, 0, 0);
insert into npc_vendor value (12785, 17578, 0, 0);
insert into npc_vendor value (12785, 17579, 0, 0);
insert into npc_vendor value (12785, 17580, 0, 0);
insert into npc_vendor value (12785, 17581, 0, 0);
insert into npc_vendor value (12785, 17583, 0, 0);
insert into npc_vendor value (12785, 17584, 0, 0);
insert into npc_vendor value (12785, 17594, 0, 0);
insert into npc_vendor value (12785, 17596, 0, 0);
insert into npc_vendor value (12785, 17598, 0, 0);
insert into npc_vendor value (12785, 17599, 0, 0);
insert into npc_vendor value (12785, 17600, 0, 0);
insert into npc_vendor value (12785, 17601, 0, 0);
insert into npc_vendor value (12785, 17602, 0, 0);
insert into npc_vendor value (12785, 17603, 0, 0);
insert into npc_vendor value (12785, 17604, 0, 0);
insert into npc_vendor value (12785, 17605, 0, 0);
insert into npc_vendor value (12785, 17607, 0, 0);
insert into npc_vendor value (12785, 17608, 0, 0);
insert into npc_vendor value (12795, 16485, 0, 0);
insert into npc_vendor value (12795, 16487, 0, 0);
insert into npc_vendor value (12795, 16489, 0, 0);
insert into npc_vendor value (12795, 16490, 0, 0);
insert into npc_vendor value (12795, 16491, 0, 0);
insert into npc_vendor value (12795, 16492, 0, 0);
insert into npc_vendor value (12795, 16494, 0, 0);
insert into npc_vendor value (12795, 16496, 0, 0);
insert into npc_vendor value (12795, 16498, 0, 0);
insert into npc_vendor value (12795, 16499, 0, 0);
insert into npc_vendor value (12795, 16501, 0, 0);
insert into npc_vendor value (12795, 16502, 0, 0);
insert into npc_vendor value (12795, 16503, 0, 0);
insert into npc_vendor value (12795, 16504, 0, 0);
insert into npc_vendor value (12795, 16505, 0, 0);
insert into npc_vendor value (12795, 16506, 0, 0);
insert into npc_vendor value (12795, 16507, 0, 0);
insert into npc_vendor value (12795, 16508, 0, 0);
insert into npc_vendor value (12795, 16509, 0, 0);
insert into npc_vendor value (12795, 16510, 0, 0);
insert into npc_vendor value (12795, 16513, 0, 0);
insert into npc_vendor value (12795, 16514, 0, 0);
insert into npc_vendor value (12795, 16515, 0, 0);
insert into npc_vendor value (12795, 16516, 0, 0);
insert into npc_vendor value (12795, 16518, 0, 0);
insert into npc_vendor value (12795, 16519, 0, 0);
insert into npc_vendor value (12795, 16521, 0, 0);
insert into npc_vendor value (12795, 16522, 0, 0);
insert into npc_vendor value (12795, 16523, 0, 0);
insert into npc_vendor value (12795, 16524, 0, 0);
insert into npc_vendor value (12795, 16525, 0, 0);
insert into npc_vendor value (12795, 16526, 0, 0);
insert into npc_vendor value (12795, 16527, 0, 0);
insert into npc_vendor value (12795, 16528, 0, 0);
insert into npc_vendor value (12795, 16530, 0, 0);
insert into npc_vendor value (12795, 16531, 0, 0);
insert into npc_vendor value (12795, 16533, 0, 0);
insert into npc_vendor value (12795, 16534, 0, 0);
insert into npc_vendor value (12795, 16535, 0, 0);
insert into npc_vendor value (12795, 16536, 0, 0);
insert into npc_vendor value (12795, 16539, 0, 0);
insert into npc_vendor value (12795, 16540, 0, 0);
insert into npc_vendor value (12795, 16541, 0, 0);
insert into npc_vendor value (12795, 16542, 0, 0);
insert into npc_vendor value (12795, 16543, 0, 0);
insert into npc_vendor value (12795, 16544, 0, 0);
insert into npc_vendor value (12795, 16545, 0, 0);
insert into npc_vendor value (12795, 16548, 0, 0);
insert into npc_vendor value (12795, 16549, 0, 0);
insert into npc_vendor value (12795, 16550, 0, 0);
insert into npc_vendor value (12795, 16551, 0, 0);
insert into npc_vendor value (12795, 16552, 0, 0);
insert into npc_vendor value (12795, 16554, 0, 0);
insert into npc_vendor value (12795, 16555, 0, 0);
insert into npc_vendor value (12795, 16558, 0, 0);
insert into npc_vendor value (12795, 16560, 0, 0);
insert into npc_vendor value (12795, 16561, 0, 0);
insert into npc_vendor value (12795, 16562, 0, 0);
insert into npc_vendor value (12795, 16563, 0, 0);
insert into npc_vendor value (12795, 16564, 0, 0);
insert into npc_vendor value (12795, 16565, 0, 0);
insert into npc_vendor value (12795, 16566, 0, 0);
insert into npc_vendor value (12795, 16567, 0, 0);
insert into npc_vendor value (12795, 16568, 0, 0);
insert into npc_vendor value (12795, 16569, 0, 0);
insert into npc_vendor value (12795, 16571, 0, 0);
insert into npc_vendor value (12795, 16573, 0, 0);
insert into npc_vendor value (12795, 16574, 0, 0);
insert into npc_vendor value (12795, 16577, 0, 0);
insert into npc_vendor value (12795, 16578, 0, 0);
insert into npc_vendor value (12795, 16579, 0, 0);
insert into npc_vendor value (12795, 16580, 0, 0);
insert into npc_vendor value (12795, 17570, 0, 0);
insert into npc_vendor value (12795, 17571, 0, 0);
insert into npc_vendor value (12795, 17572, 0, 0);
insert into npc_vendor value (12795, 17573, 0, 0);
insert into npc_vendor value (12795, 17576, 0, 0);
insert into npc_vendor value (12795, 17577, 0, 0);
insert into npc_vendor value (12795, 17586, 0, 0);
insert into npc_vendor value (12795, 17588, 0, 0);
insert into npc_vendor value (12795, 17590, 0, 0);
insert into npc_vendor value (12795, 17591, 0, 0);
insert into npc_vendor value (12795, 17592, 0, 0);
insert into npc_vendor value (12795, 17593, 0, 0);
insert into npc_vendor value (12795, 17610, 0, 0);
insert into npc_vendor value (12795, 17611, 0, 0);
insert into npc_vendor value (12795, 17612, 0, 0);
insert into npc_vendor value (12795, 17613, 0, 0);
insert into npc_vendor value (12795, 17616, 0, 0);
insert into npc_vendor value (12795, 17617, 0, 0);
insert into npc_vendor value (12795, 17618, 0, 0);
insert into npc_vendor value (12795, 17620, 0, 0);
insert into npc_vendor value (12795, 17622, 0, 0);
insert into npc_vendor value (12795, 17623, 0, 0);
insert into npc_vendor value (12795, 17624, 0, 0);
insert into npc_vendor value (12795, 17625, 0, 0);

-- http://wowwiki.wikia.com/wiki/Patch_1.11.0
-- Class specific enchantments given by Zanza the Restless no longer require Arcanum (Librams) from Dire Maul. 
-- The requirements have been changed to items found within Zul'Gurub. Speak with Zanza the Restless for more information.
UPDATE quest_template    SET ReqItemId3 = 18331, ReqItemCount3 = 1,    ReqItemId4 = 19716, ReqItemCount4 = 1    WHERE entry = 8192;
UPDATE quest_template    SET ReqItemId3 = 18329, ReqItemCount3 = 1,    ReqItemId4 = 19723, ReqItemCount4 = 1    WHERE entry = 8186;
UPDATE quest_template    SET ReqItemId3 = 18329, ReqItemCount3 = 1,    ReqItemId4 = 19718, ReqItemCount4 = 1    WHERE entry = 8187;
UPDATE quest_template    SET ReqItemId3 = 18331, ReqItemCount3 = 1,    ReqItemId4 = 19724, ReqItemCount4 = 1    WHERE entry = 8184;
UPDATE quest_template    SET ReqItemId3 = 18330, ReqItemCount3 = 1,    ReqItemId4 = 19719, ReqItemCount4 = 1    WHERE entry = 8189;
UPDATE quest_template    SET ReqItemId3 = 18330, ReqItemCount3 = 1,    ReqItemId4 = 19722, ReqItemCount4 = 1    WHERE entry = 8191;
UPDATE quest_template    SET ReqItemId3 = 18331, ReqItemCount3 = 1,    ReqItemId4 = 19721, ReqItemCount4 = 1    WHERE entry = 8185;
UPDATE quest_template    SET ReqItemId3 = 18330, ReqItemCount3 = 1,    ReqItemId4 = 19720, ReqItemCount4 = 1    WHERE entry = 8188;
UPDATE quest_template    SET ReqItemId3 = 18330, ReqItemCount3 = 1,    ReqItemId4 = 19717, ReqItemCount4 = 1    WHERE entry = 8190;
DELETE FROM item_template WHERE entry=22637;
UPDATE quest_template SET ReqItemId1 = 0, ReqItemCount1 = 0 WHERE entry IN (8184, 8185, 8186, 8187, 8188, 8189, 8190, 8191, 8192);
