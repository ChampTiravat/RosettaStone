# How to add card

* [Step 1: Select card](#step-1-select-card)
* [Step 2: Add card to CardSets](#step-2-add-card-to-cardsets)
  * [Cards with power](#cards-with-power)
* [Step 3: Add test code](#step-3-add-test-code)

## Step 1: Select card

The first thing you have to do is to select the cards to add.

See [CardList.md](./CardList.md) for all card list.

* [Basic card list](./CardList.md#basic)
* [Classic card list](./CardList.md#classic)

Select a card that is not yet implemented in card list.

Let's assume that you have selected the **CS2_124 ("Wolfrider")** card.

## Step 2: Add card to CardSets

The next thing to do is to add selected card to the card set. There are C++ source files for each card set in `Sources/Rosetta/CardSets`.
You need to select right source file by card set.
See [CardList.md](./CardList.md) and watch `Set` label.
Card `CS2_124` that you will add is a `CORE` set card. So this card should be added to `CoreCardsGen.cpp`

The main code for `CoreCardsGen.cpp` is as follows.

```C++
void CoreCardsGen::AddAll(std::map<std::string, Power*>& cards)
{
    AddHeroes(cards);
    AddHeroPowers(cards);

    AddDruid(cards);
    AddDruidNonCollect(cards);

    AddHunter(cards);
    AddHunterNonCollect(cards);

    AddMage(cards);
    AddMageNonCollect(cards);

    AddPaladin(cards);
    AddPaladinNonCollect(cards);

    AddPriest(cards);
    AddPriestNonCollect(cards);

    AddRogue(cards);
    AddRogueNonCollect(cards);

    AddShaman(cards);
    AddShamanNonCollect(cards);

    AddWarlock(cards);
    AddWarlockNonCollect(cards);

    AddWarrior(cards);
    AddWarriorNonCollect(cards);

    AddNeutral(cards);
    AddNeutralNonCollect(cards);
}
```

Now you need to add a card to appropriate function depending on the card class. Do you not know what class of card is? Visit [Hearthstone Wiki](https://hearthstone.gamepedia.com/Hearthstone_Wiki) and search by card name. Search results for [Wolfrider](https://hearthstone.gamepedia.com/Wolfrider) are:

<img src="https://media-hearth.cursecdn.com/avatars/330/147/174.png">

```
Wolfrider is a neutral minion card, from the Basic set.
```

So you can add a card to the `AddNeutral` function.

**NOTE: All of the cards in card list can be collected. Therefore, it doesn't add to AddXXXNonCollect function.**

Wolfrider is a minion with the **Charge** ability. [Charge](https://hearthstone.gamepedia.com/Charge) is an ability allowing a minion to attack the same turn it is summoned or brought under a new player's control. The default abilities such as **Charge**, **Windfury** are saved as GameTag when parsing card data, so you don't need to add power separately. Therefore, you can add a card like this:

```C++
void CoreCardsGen::AddNeutral(std::map<std::string, Power*>& cards)
{
    Power power;

    // --------------------------------------- MINION - NEUTRAL
    // [CS2_124] Wolfrider - COST:3 [ATK:3/HP:1]
    // - Set: Core, Rarity: Free
    // --------------------------------------------------------
    // Text: <b>Charge</b>
    // --------------------------------------------------------
    // GameTag:
    // - CHARGE = 1
    // --------------------------------------------------------
    power.ClearData();
    power.AddPowerTask(nullptr);
    cards.emplace("CS2_124", nullptr);
}
```

Note that the first parameter is card ID and the second parameter is card power.

That's it! Now, it's time to test the card to see if it works.

**NOTE: There are many abilities that are not yet implemented. A list of currently implemented abilities can be found in [AbilityList.md](./AbilityList.md). If selected card is required an ability to be implemented, please ask collaborators([utilForever](https://github.com/utilForever), [revsic](https://github.com/revsic), [FuZer](https://github.com/FuZer)).**

### Cards with power

Some cards such as **CS2_041 ("Ancestral Healing")** require power.

<img src="https://media-hearth.cursecdn.com/avatars/329/970/216.png">

```
Ancestral Healing is a shaman spell card, from the Basic set. It restores a minion to their current maximum Health value and grants it the Taunt ability.
```

This card has two powers: One is the power to restore current maximum health, and the other is the power to grant the Taunt ability. Also, this card is a shaman spell card. So you can add a card to the `AddShaman` function.

```C++
void CoreCardsGen::AddShaman(std::map<std::string, Power*>& cards)
{
    // ----------------------------------------- SPELL - SHAMAN
    // [CS2_041] Ancestral Healing - COST:0
    // - Faction: Neutral, Set: Basic, Rarity: Free
    // --------------------------------------------------------
    // Text: Restore a minion
    //       to full Health and
    //       give it <b>Taunt</b>.
    // --------------------------------------------------------
    // PlayReq:
    // - REQ_TARGET_TO_PLAY = 0
    // - REQ_MINION_TARGET = 0
    // --------------------------------------------------------
    // Tag:
    // - TAUNT = 1
    // --------------------------------------------------------
    Power* p = new Power;
    p->powerTask.emplace_back(new PowerTask::HealFullTask(EntityType::TARGET));
    p->powerTask.emplace_back(new PowerTask::AddEnchantmentTask("CS2_041e", EntityType::TARGET));
    cards.emplace("CS2_041", p);
}
```

Meanwhile, The power to grant the Taunt ability is the enchantment of Ancestral Healing. This enchantment is called **CS2_041e ("Ancestral Infusion")**. Since this card can't be collected, add it to `ShamanNonCollect` function.

```C++
void CoreCardsGen::AddShamanNonCollect(std::map<std::string, Power*>& cards)
{
    // ----------------------------------- ENCHANTMENT - SHAMAN
    // [CS2_041e] Ancestral Infusion (*) - COST:0
    // - Set: Core
    // --------------------------------------------------------
    // Text: Taunt.
    // --------------------------------------------------------
    // GameTag:
    // - TAUNT = 1
    // --------------------------------------------------------
    Power* p = new Power;
    p->enchant = new Enchant(Effects::Taunt);
    cards.emplace("CS2_041e", p);
}
```

**NOTE: There are many tasks that are not yet implemented. A list of currently implemented tasks can be found in [TaskList.md](./TaskList.md). If selected card is required a task to be implemented, please ask collaborators([utilForever](https://github.com/utilForever), [revsic](https://github.com/revsic), [FuZer](https://github.com/FuZer)).**

## Step 3: Add test code

The last thing you need to do is test card you've added. There are many test files for each card in `Tests/UnitTests/CardSets`. Card `CS2_124` that you will add is a basic card set. So, create `CS2_124.cpp` file in `Basic` folder.

Test file has the following structure:

```C++
#include <Utils/CardSetUtils.h>

TEST(CoreCardsGen, CS2_124)
{
    ...
}
```

To test card, you need to consider play scenario. Also, you need to consider the abilities and powers that the cards have. For example, Wolfrider has **Charge** ability.

```
Charge is an ability allowing a minion to attack the same turn it is summoned or brought under a new player's control.
```

Therefore, you need to create a scenario to ensure that this ability works properly. An example of a scenario is:

```
1. Current player draws "Acidic Swamp Ooze" card.
2. Current player plays "Acidic Swap Ooze" card.
3. Current player ends turn.
4. Opponent player draws "Wolfrider" card.
5. Opponent player plays "Wolfrider" card.
6. Opponent player attacks "Acidic Swamp Ooze" with "Wolfrider".
```

It is important to check whether the processing is normal after each operation. For example, after current player draws "Acidic Swamp Ooze" card, you make sure that there is a card in hand and that card is "Acidic Swamp Ooze". In addition, after opponent player attacks "Acidic Swamp Ooze" with "Wolfrider", you make sure that there are no cards in the field. Let's write test code based on this scenario.

First, you should configure the scenario environment. `Game` is a class that stores player manages game state.

```C++
GameConfig config;
config.player1Class = CardClass::WARRIOR;
config.player2Class = CardClass::ROGUE;
config.startPlayer = PlayerType::PLAYER1;

Game game(config);
game.StartGame();

Player& curPlayer = game.GetCurrentPlayer();
Player& opPlayer = game.GetCurrentPlayer().GetOpponent();
curPlayer.maximumMana = 10;
curPlayer.currentMana = 10;
opPlayer.maximumMana = 10;
opPlayer.currentMana = 10;
```

Then, you should perform operations according to the scenario and test it.

```C++
TEST(CoreCardsGen, CS2_124)
{
    GameConfig config;
    config.player1Class = CardClass::WARRIOR;
    config.player2Class = CardClass::ROGUE;
    config.startPlayer = PlayerType::PLAYER1;

    Game game(config);
    game.StartGame();
    game.ProcessUntil(Step::MAIN_START);    

    Player& curPlayer = game.GetCurrentPlayer();
    Player& opPlayer = game.GetCurrentPlayer().GetOpponent();
    curPlayer.maximumMana = 10;
    curPlayer.currentMana = 10;
    opPlayer.maximumMana = 10;
    opPlayer.currentMana = 10;

    // 1. Current player draws "Acidic Swamp Ooze" card.
    const auto card1 = Generic::DrawCard(
        curPlayer, Cards::GetInstance().FindCardByName("Acidic Swamp Ooze"));

    // 2. Current player plays "Acidic Swap Ooze" card.
    Task::Run(curPlayer, PlayCardTask::Minion(curPlayer, card1));

    // 3. Current player ends turn.
    Task::Run(curPlayer, EndTurnTask());
    game.ProcessUntil(Step::MAIN_START);

    // 4. Opponent player draws "Wolfrider" card.
    const auto card2 = Generic::DrawCard(
        opPlayer, Cards::GetInstance().FindCardByName("Wolfrider"));

    // 5. Opponent player plays "Wolfrider" card.
    Task::Run(opPlayer, PlayCardTask::Minion(opPlayer, card2));

    // 6. Opponent player attacks "Acidic Swamp Ooze" with "Wolfrider".
    Task::Run(curPlayer, AttackTask(card2, card1));
    EXPECT_EQ(curPlayer.GetField().GetNumOfMinions(), 0u);
    EXPECT_EQ(opPlayer.GetField().GetNumOfMinions(), 0u);
}
```

When you have finished writing test code, compile and build it. And you have to make sure the test passes.

```
[==========] Running 77 tests from 16 test cases.
[----------] Global test environment set-up.

...

[----------] 1 tests from CoreCardsGen
[ RUN      ] CoreCardsGen.CS2_124
[       OK ] CoreCardsGen.CS2_124 (0 ms)
[----------] 1 tests from CoreCardsGen (0 ms total)

...

[----------] Global test environment tear-down
[==========] 77 tests from 16 test cases ran. (501 ms total)
[  PASSED  ] 77 tests.
```

Good! Now, [let's do a pull request](./PullRequests.md).