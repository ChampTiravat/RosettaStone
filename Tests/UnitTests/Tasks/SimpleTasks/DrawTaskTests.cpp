// Copyright (c) 2018 Chris Ohk, Youngjoong Kim, SeungHyun Jeon

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include "gtest/gtest.h"

#include <hspp/Cards/Cards.hpp>
#include <hspp/Commons/Utils.hpp>
#include <hspp/Models/Game.hpp>
#include <hspp/Policy/BasicPolicy.hpp>
#include <hspp/Tasks/SimpleTasks/DrawTask.hpp>

using namespace Hearthstonepp;
using namespace SimpleTasks;

class DrawTestPolicy : public BasicPolicy
{
 public:
    DrawTestPolicy(std::function<void(const TaskMeta&)>&& overdrawHandler)
        : m_overdrawHandler(std::move(overdrawHandler))
    {
    }

    void NotifyOverDraw(const TaskMeta& meta) override
    {
        m_overdrawHandler(meta);
    }

 private:
    std::function<void(const TaskMeta&)> m_overdrawHandler;
};

TEST(DrawTask, GetTaskID)
{
    const DrawTask draw(1);

    EXPECT_EQ(draw.GetTaskID(), +TaskID::DRAW);
}

TEST(DrawTask, Run)
{
    std::vector<Card> cards;
    std::vector<Entity*> entities;

    const auto Generate = [&](std::string&& id) -> Entity* {
        cards.emplace_back(Card());

        Card card = cards.back();
        card.id = std::move(id);

        entities.emplace_back(new Entity(card));

        return entities.back();
    };

    Game game(CardClass::ROGUE, CardClass::DRUID, PlayerType::PLAYER1);
    Player& p = game.GetPlayer1();

    const std::string id = "card";
    for (char i = '0'; i < '3'; ++i)
    {
        p.GetDeck().emplace_back(Generate(id + i));
    }

    DrawTask draw(3);
    TaskStatus result = draw.Run(p);
    EXPECT_EQ(result, TaskStatus::DRAW_SUCCESS);
    EXPECT_EQ(p.GetHand().size(), static_cast<size_t>(3));

    for (size_t i = 0; i < 3; ++i)
    {
        EXPECT_EQ(p.GetHand()[i]->card->id,
                  id + static_cast<char>(2 - i + 0x30));
    }
}

TEST(DrawTask, RunExhaust)
{
    Game game(CardClass::ROGUE, CardClass::DRUID, PlayerType::PLAYER1);
    Player& p = game.GetPlayer1();
    EXPECT_EQ(p.GetDeck().size(), static_cast<size_t>(0));

    DrawTask draw(3);

    TaskStatus result = draw.Run(game.GetPlayer1());
    EXPECT_EQ(result, TaskStatus::DRAW_EXHAUST);
    EXPECT_EQ(p.GetHand().size(), static_cast<size_t>(0));
    EXPECT_EQ(p.GetDeck().size(), static_cast<size_t>(0));
    EXPECT_EQ(p.GetNumCardAfterExhaust(), 3);
    // Health: 30 - (1 + 2 + 3)
    EXPECT_EQ(p.GetHero()->health, static_cast<size_t>(24));

    Card card;
    card.id = "card1";

    auto entity = new Entity(card);
    p.GetDeck().emplace_back(entity);

    result = draw.Run(game.GetPlayer1());
    EXPECT_EQ(result, TaskStatus::DRAW_EXHAUST);
    EXPECT_EQ(p.GetHand().size(), static_cast<size_t>(1));
    EXPECT_EQ(p.GetHand()[0]->card->id, "card1");
    EXPECT_EQ(p.GetDeck().size(), static_cast<size_t>(0));
    EXPECT_EQ(p.GetNumCardAfterExhaust(), 5);
    // Health: 30 - (1 + 2 + 3 + 4 + 5)
    EXPECT_EQ(p.GetHero()->health, static_cast<size_t>(15));
}

TEST(DrawTask, RunOverDraw)
{
    std::vector<Card> cards;
    std::vector<Entity*> entities;

    const auto Generate = [&](std::string&& id) -> Entity* {
        cards.emplace_back(Card());

        Card card = cards.back();
        card.id = std::move(id);

        entities.emplace_back(new Entity(card));

        return entities.back();
    };

    Game game(CardClass::ROGUE, CardClass::DRUID, PlayerType::PLAYER1);
    Player& p = game.GetPlayer1();

    const std::string id = "card";
    for (char i = '0'; i < '3'; ++i)
    {
        p.GetDeck().emplace_back(Generate(id + i));
    }

    p.GetHand().resize(10);

    DrawTask draw(3);

    DrawTestPolicy policy([&](const TaskMeta& burnt) {
        EXPECT_EQ(burnt.GetID(), +TaskID::OVERDRAW);
        EXPECT_EQ(burnt.GetStatus(), TaskStatus::DRAW_OVERDRAW);
        EXPECT_EQ(burnt.GetUserID(), p.GetID());

        EXPECT_TRUE(burnt.HasObjects());

        const Box<Entity*>& entities = burnt.GetObject<Box<Entity*>>();
        for (size_t i = 0; i < 3; ++i)
        {
            EXPECT_EQ(entities[i]->card->id,
                      id + static_cast<char>(2 - i + 0x30));
        }
    });
    p.SetPolicy(&policy);

    TaskStatus result = draw.Run(p);
    EXPECT_EQ(result, TaskStatus::DRAW_OVERDRAW);
    EXPECT_EQ(p.GetDeck().size(), static_cast<size_t>(0));
    EXPECT_EQ(p.GetHand().size(), static_cast<size_t>(10));
}

TEST(DrawTask, RunExhaustOverdraw)
{
    std::vector<Card> cards;
    std::vector<Entity*> entities;

    const auto Generate = [&](std::string&& id) -> Entity* {
        cards.emplace_back(Card());

        Card card = cards.back();
        card.id = std::move(id);

        entities.emplace_back(new Entity(card));
        return entities.back();
    };

    Game game(CardClass::ROGUE, CardClass::DRUID, PlayerType::PLAYER1);
    Player& p = game.GetPlayer1();

    const std::string id = "card";
    for (char i = '0'; i < '3'; ++i)
    {
        p.GetDeck().emplace_back(Generate(id + i));
    }

    p.GetHand().resize(9);

    DrawTask draw(4);

    DrawTestPolicy policy([&](const TaskMeta& burnt) {
        EXPECT_EQ(burnt.GetID(), +TaskID::OVERDRAW);
        EXPECT_EQ(burnt.GetStatus(), TaskStatus::DRAW_OVERDRAW);
        EXPECT_EQ(burnt.GetUserID(), p.GetID());

        EXPECT_TRUE(burnt.HasObjects());

        const Box<Entity*>& entities = burnt.GetObject<Box<Entity*>>();
        for (size_t i = 0; i < 2; ++i)
        {
            EXPECT_EQ(entities[i]->card->id,
                        id + static_cast<char>(2 - i + 0x30));
        }
    });
    p.SetPolicy(&policy);

    TaskStatus result = draw.Run(p);
    EXPECT_EQ(result, TaskStatus::DRAW_EXHAUST_OVERDRAW);
    EXPECT_EQ(p.GetDeck().size(), static_cast<size_t>(0));
    EXPECT_EQ(p.GetHand().size(), static_cast<size_t>(10));
    EXPECT_EQ(p.GetHand()[9]->card->id, "card0");
}

TEST(DrawCardTask, GetTaskID)
{
    const Card card{};
    const DrawCardTask draw(card);

    EXPECT_EQ(draw.GetTaskID(), +TaskID::DRAW);
}

TEST(DrawCardTask, Run)
{
    Cards& instance = Cards::GetInstance();
    Game game(CardClass::ROGUE, CardClass::DRUID, PlayerType::PLAYER1);

    Card nerubian = instance.FindCardByID("AT_036t");
    EXPECT_NE(nerubian.id, "");
    EXPECT_EQ(nerubian.name, "Nerubian");

    Card poisonedBlade = instance.FindCardByID("AT_034");
    EXPECT_NE(poisonedBlade.id, "");
    EXPECT_EQ(poisonedBlade.name, "Poisoned Blade");

    auto minionNerubian = new Entity(nerubian);
    auto weaponPoisonedBlade = new Entity(poisonedBlade);
    game.GetPlayer1().GetDeck().emplace_back(weaponPoisonedBlade);
    game.GetPlayer1().GetDeck().emplace_back(minionNerubian);

    DrawCardTask drawNerubian(nerubian);
    TaskStatus result = drawNerubian.Run(game.GetPlayer1());

    EXPECT_EQ(result, TaskStatus::DRAW_SUCCESS);
    EXPECT_EQ(game.GetPlayer1().GetHand().size(), static_cast<size_t>(1));
    EXPECT_EQ(game.GetPlayer1().GetHand()[0]->card->id, nerubian.id);
    EXPECT_EQ(game.GetPlayer1().GetDeck().size(), static_cast<size_t>(1));
    EXPECT_EQ(game.GetPlayer1().GetDeck()[0]->card->id, poisonedBlade.id);

    DrawCardTask drawPoisonedBlade(poisonedBlade);
    result = drawPoisonedBlade.Run(game.GetPlayer1());

    EXPECT_EQ(result, TaskStatus::DRAW_SUCCESS);
    EXPECT_EQ(game.GetPlayer1().GetHand().size(), static_cast<size_t>(2));
    EXPECT_EQ(game.GetPlayer1().GetHand()[1]->card->id, poisonedBlade.id);
    EXPECT_EQ(game.GetPlayer1().GetDeck().size(), static_cast<size_t>(0));

    delete minionNerubian;
    delete weaponPoisonedBlade;
}
