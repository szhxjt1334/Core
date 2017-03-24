-- Patch 1.6 World DB Itemization Changes.
-- To be applied on QA first for testing.

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

-- Remove Class Quests for the Warrior, Shaman, Paladin, and Warlock
DELETE FROM quest_template WHERE entry IN (
-- Warrior:
8417, -- http://wowwiki.wikia.com/Quest:A_Troubled_Spirit
8423, -- http://wowwiki.wikia.com/Quest:Warrior_Kinship
8424, -- http://wowwiki.wikia.com/Quest:War_on_the_Shadowsworn
8425, -- http://wowwiki.wikia.com/Quest:Voodoo_Feathers
-- Warlock:
8419, -- http://wowwiki.wikia.com/Quest:An_Imp%27s_Request
8421, -- http://wowwiki.wikia.com/Quest:The_Wrong_Stuff
8422, -- http://wowwiki.wikia.com/Quest:Trolls_of_a_Feather
-- Shaman:
8410, -- http://wowwiki.wikia.com/Quest:Elemental_Mastery
8412, -- http://wowwiki.wikia.com/Quest:Spirit_Totem
8413, -- http://wowwiki.wikia.com/Quest:Da_Voodoo
-- Paladin:
8415, -- http://wowwiki.wikia.com/Quest:Chillwind_Camp
8414, -- http://wowwiki.wikia.com/Quest:Dispelling_Evil
8416, -- http://wowwiki.wikia.com/Quest:Inert_Scourgestones
8418  -- http://wowwiki.wikia.com/Quest:Forging_the_Mightstone
);

-- [Glyph Chasing]
DELETE from quest_template WHERE entry=8309;

-- Fix Alterac Valley Kazzak quest: non existant reward removed.
UPDATE quest_template SET RewChoiceItemId4 = 0 WHERE entry IN (7202, 7181);

-- Sunken Temple Level 50 Quests
DELETE FROM quest_template WHERE entry IN (8418, 8413, 8422, 8425);

-- Disable not in the game recipe + resulting food spell.
INSERT INTO spell_disabled SET entry=24800;
INSERT INTO spell_disabled SET entry=24801;

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

UPDATE quest_template SET ReqItemId1 = 0, ReqItemCount1 = 0 WHERE entry IN (8184, 8185, 8186, 8187, 8188, 8189, 8190, 8191, 8192);
