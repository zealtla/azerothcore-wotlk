
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "GameEventMgr.h"
#include "Player.h"
#include "WorldSession.h"
#pragma execution_character_set("utf-8")

enum eTrickOrTreatSpells
{
    SPELL_TRICK                 = 24714,
    SPELL_TREAT                 = 24715,
    SPELL_TRICKED_OR_TREATED    = 24755,
    HALLOWEEN_EVENTID           = 12,
};

class npc_innkeeper : public CreatureScript
{
public:
    npc_innkeeper() : CreatureScript("npc_innkeeper") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (IsEventActive(HALLOWEEN_EVENTID) && !player->HasAura(SPELL_TRICKED_OR_TREATED))
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Trick or Treat!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+HALLOWEEN_EVENTID);

        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsVendor())
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        if (creature->IsInnkeeper())
            AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, "Make this inn my home.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INN);

        player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        ClearGossipMenuFor(player);
        if (action == GOSSIP_ACTION_INFO_DEF+HALLOWEEN_EVENTID && IsEventActive(HALLOWEEN_EVENTID) && !player->HasAura(SPELL_TRICKED_OR_TREATED))
        {
            player->CastSpell(player, SPELL_TRICKED_OR_TREATED, true);
            creature->CastSpell(player, roll_chance_i(50) ? SPELL_TRICK : SPELL_TREAT, true);

            CloseGossipMenuFor(player);
            return true;
        }

        CloseGossipMenuFor(player);

        switch (action)
        {
            case GOSSIP_ACTION_TRADE: player->GetSession()->SendListInventory(creature->GetGUID()); break;
            case GOSSIP_ACTION_INN: player->SetBindPoint(creature->GetGUID()); break;
        }
        return true;
    }
};

//多商NPC entry:200000
class npc_newbie : public CreatureScript
{
public:
    npc_newbie() : CreatureScript("npc_newbie") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "新人装备", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "新人武器", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "新手饰品", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3); 
        
        player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        ClearGossipMenuFor(player);
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF + 1: player->GetSession()->SendListInventory(creature->GetGUID(), 200001); break; 
        case GOSSIP_ACTION_INFO_DEF + 2: player->GetSession()->SendListInventory(creature->GetGUID(), 200002); break;
        case GOSSIP_ACTION_INFO_DEF + 3: player->GetSession()->SendListInventory(creature->GetGUID(), 200003); break;
        }
        return true;
    }
};

void AddSC_npc_innkeeper()
{
    new npc_innkeeper;
    new npc_newbie;
}

