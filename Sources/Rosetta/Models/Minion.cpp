// Copyright (c) 2019 Chris Ohk, Youngjoong Kim, SeungHyun Jeon

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <Rosetta/Games/Game.hpp>
#include <Rosetta/Models/Minion.hpp>
#include <Rosetta/Models/Player.hpp>

#include <utility>

namespace RosettaStone
{
Minion::Minion(Player& _owner, Card& _card, std::map<GameTag, int> tags)
    : Character(_owner, _card, std::move(tags))
{
    // Do nothing
}

int Minion::GetLastBoardPos() const
{
    return GetGameTag(GameTag::TAG_LAST_KNOWN_COST_IN_HAND);
}

void Minion::SetLastBoardPos(int value)
{
    SetGameTag(GameTag::TAG_LAST_KNOWN_COST_IN_HAND, value);
}

void Minion::Reset()
{
    Entity::Reset();

    if (onGoingEffect != nullptr)
    {
        onGoingEffect->Remove();
    }

    if (isDestroyed)
    {
        auto iter = owner->GetGame()->deadMinions.find(orderOfPlay);
        if (iter != owner->GetGame()->deadMinions.end())
        {
            owner->GetGame()->deadMinions.erase(iter);
        }

        isDestroyed = false;
    }
}

void Minion::Destroy()
{
    Entity::Destroy();

    owner->GetGame()->deadMinions.emplace(orderOfPlay, this);
}
}  // namespace RosettaStone
