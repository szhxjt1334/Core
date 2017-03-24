
-- http://wowwiki.wikia.com/Plans:_Nightfall

DELETE FROM quest_template WHERE entry=19212;

-- Darkmoon Faire quests

DELETE FROM creature_involvedrelation WHERE quest IN (
7928, -- http://wowwiki.wikia.com/wiki/Quest:Darkmoon_Warlords_Deck
7927, -- http://wowwiki.wikia.com/wiki/Quest:Darkmoon_Portals_Deck
7929, -- http://wowwiki.wikia.com/wiki/Quest:Darkmoon_Elementals_Deck
7907, -- http://wowwiki.wikia.com/wiki/Quest:Darkmoon_Beast_Deck
7935, -- 10 Tickets - Last Month's Mutton
7932, -- 12 Tickets - Lesser Darkmoon Prize
7981, -- 1200 Tickets - Amulet of the Darkmoon
7940, -- 1200 Tickets - Orb of the Darkmoon
7933, -- 40 Tickets - Greater Darkmoon Prize
7930, -- 5 Tickets - Darkmoon Flower
7931, -- 5 Tickets - Minor Darkmoon Prize
7934, -- 50 Tickets - Darkmoon Storage Box
7936, -- 50 Tickets - Last Year's Mutton
7885, -- Armor Kits
7892, -- Big Black Mace
7881, -- Carnival Boots
7882, -- Carnival Jerkins
7889, -- Coarse Weightstone
7894, -- Copper Modulator
7884, -- Crocolisk Boy and the Bearded Murloc
7903, -- Evil Bat Eyes
8222, -- Glowing Scorpid Blood
7896, -- Green Fireworks
7891, -- Green Iron Bracers
7890, -- Heavy Grinding Stone
7897, -- Mechanical Repair Kits
7941, -- More Armor Kits
7943, -- More Bat Eyes
7939, -- More Dense Grinding Stones
8223, -- More Glowing Scorpid Blood
7942, -- More Thorium Widgets
7893, -- Rituals of Strength
7899, -- Small Furry Paws
7901, -- Soft Bushy Tails
7946, -- Spawn of Jubjub
7883, -- The World's Largest Gnome!
7898, -- Thorium Widget
7900, -- Torn Bear Pelts
7902, -- Vibrant Plumes
7895, -- Whirring Bronze Gizmo
7937, -- Your Fortune Awaits You...
7938, -- Your Fortune Awaits You...
7944, -- Your Fortune Awaits You...
7945 -- Your Fortune Awaits You...
);

DELETE FROM creature_questrelation WHERE quest IN (
7928, -- http://wowwiki.wikia.com/wiki/Quest:Darkmoon_Warlords_Deck
7927, -- http://wowwiki.wikia.com/wiki/Quest:Darkmoon_Portals_Deck
7929, -- http://wowwiki.wikia.com/wiki/Quest:Darkmoon_Elementals_Deck
7907, -- http://wowwiki.wikia.com/wiki/Quest:Darkmoon_Beast_Deck
7935, -- 10 Tickets - Last Month's Mutton
7932, -- 12 Tickets - Lesser Darkmoon Prize
7981, -- 1200 Tickets - Amulet of the Darkmoon
7940, -- 1200 Tickets - Orb of the Darkmoon
7933, -- 40 Tickets - Greater Darkmoon Prize
7930, -- 5 Tickets - Darkmoon Flower
7931, -- 5 Tickets - Minor Darkmoon Prize
7934, -- 50 Tickets - Darkmoon Storage Box
7936, -- 50 Tickets - Last Year's Mutton
7885, -- Armor Kits
7892, -- Big Black Mace
7881, -- Carnival Boots
7882, -- Carnival Jerkins
7889, -- Coarse Weightstone
7894, -- Copper Modulator
7884, -- Crocolisk Boy and the Bearded Murloc
7903, -- Evil Bat Eyes
8222, -- Glowing Scorpid Blood
7896, -- Green Fireworks
7891, -- Green Iron Bracers
7890, -- Heavy Grinding Stone
7897, -- Mechanical Repair Kits
7941, -- More Armor Kits
7943, -- More Bat Eyes
7939, -- More Dense Grinding Stones
8223, -- More Glowing Scorpid Blood
7942, -- More Thorium Widgets
7893, -- Rituals of Strength
7899, -- Small Furry Paws
7901, -- Soft Bushy Tails
7946, -- Spawn of Jubjub
7883, -- The World's Largest Gnome!
7898, -- Thorium Widget
7900, -- Torn Bear Pelts
7902, -- Vibrant Plumes
7895, -- Whirring Bronze Gizmo
7937, -- Your Fortune Awaits You...
7938, -- Your Fortune Awaits You...
7944, -- Your Fortune Awaits You...
7945 -- Your Fortune Awaits You...
);

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

-- Run query to remove unsing triggers from DB

DELETE FROM areatrigger_involvedrelation WHERE quest NOT IN (SELECT entry FROM quest_template);
DELETE FROM creature_involvedrelation WHERE quest NOT IN (SELECT entry FROM quest_template);
DELETE FROM creature_questrelation WHERE quest NOT IN (SELECT entry FROM quest_template);
DELETE FROM gameobject_involvedrelation WHERE quest NOT IN (SELECT entry FROM quest_template);
DELETE FROM gameobject_questrelation WHERE quest NOT IN (SELECT entry FROM quest_template);
