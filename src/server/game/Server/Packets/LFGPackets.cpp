/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "LFGPackets.h"

namespace WorldPackets::LFG
{
void DFJoin::Read()
{
    QueueAsGroup = _worldPacket.ReadBit();
    bool hasPartyIndex = _worldPacket.ReadBit();
    Mercenary = _worldPacket.ReadBit();
    _worldPacket >> Roles;
    Slots.resize(_worldPacket.read<uint32>());
    if (hasPartyIndex)
        _worldPacket >> PartyIndex.emplace();
    for (uint32& slot : Slots)
        _worldPacket >> slot;
}

void DFLeave::Read()
{
    _worldPacket >> Ticket;
}

void DFProposalResponse::Read()
{
    _worldPacket >> Ticket;
    _worldPacket >> InstanceID;
    _worldPacket >> ProposalID;
    Accepted = _worldPacket.ReadBit();
}

void DFSetRoles::Read()
{
    bool hasPartyIndex = _worldPacket.ReadBit();
    _worldPacket >> RolesDesired;
    if (hasPartyIndex)
        _worldPacket >> PartyIndex.emplace();
}

void DFBootPlayerVote::Read()
{
    Vote = _worldPacket.ReadBit();
}

void DFTeleport::Read()
{
    TeleportOut = _worldPacket.ReadBit();
}

void DFGetSystemInfo::Read()
{
    Player = _worldPacket.ReadBit();
    if (_worldPacket.ReadBit())
        _worldPacket >> PartyIndex.emplace();
}

ByteBuffer& operator<<(ByteBuffer& data, LFGBlackListSlot const& lfgBlackListSlot)
{
    data << uint32(lfgBlackListSlot.Slot);
    data << uint32(lfgBlackListSlot.Reason);
    data << int32(lfgBlackListSlot.SubReason1);
    data << int32(lfgBlackListSlot.SubReason2);
    data << uint32(lfgBlackListSlot.SoftLock);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, LFGBlackList const& blackList)
{
    data.WriteBit(blackList.PlayerGuid.has_value());
    data << uint32(blackList.Slot.size());
    if (blackList.PlayerGuid)
        data << *blackList.PlayerGuid;

    for (LFGBlackListSlot const& slot : blackList.Slot)
        data << slot;

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, LfgPlayerQuestRewardItem const& playerQuestRewardItem)
{
    data << int32(playerQuestRewardItem.ItemID);
    data << int32(playerQuestRewardItem.Quantity);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, LfgPlayerQuestRewardCurrency const& playerQuestRewardCurrency)
{
    data << int32(playerQuestRewardCurrency.CurrencyID);
    data << int32(playerQuestRewardCurrency.Quantity);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, LfgPlayerQuestReward const& playerQuestReward)
{
    data << uint8(playerQuestReward.Mask);
    data << int32(playerQuestReward.RewardMoney);
    data << int32(playerQuestReward.RewardXP);
    data << uint32(playerQuestReward.Item.size());
    data << uint32(playerQuestReward.Currency.size());
    data << uint32(playerQuestReward.BonusCurrency.size());

    for (LfgPlayerQuestRewardItem const& item : playerQuestReward.Item)
        data << item;

    for (LfgPlayerQuestRewardCurrency const& currency : playerQuestReward.Currency)
        data << currency;

    for (LfgPlayerQuestRewardCurrency const& bonusCurrency : playerQuestReward.BonusCurrency)
        data << bonusCurrency;

    data.WriteBit(playerQuestReward.RewardSpellID.has_value());
    data.WriteBit(playerQuestReward.ArtifactXPCategory.has_value());
    data.WriteBit(playerQuestReward.ArtifactXP.has_value());
    data.WriteBit(playerQuestReward.Honor.has_value());
    data.FlushBits();

    if (playerQuestReward.RewardSpellID)
        data << int32(*playerQuestReward.RewardSpellID);

    if (playerQuestReward.ArtifactXPCategory)
        data << int32(*playerQuestReward.ArtifactXPCategory);

    if (playerQuestReward.ArtifactXP)
        data << uint64(*playerQuestReward.ArtifactXP);

    if (playerQuestReward.Honor)
        data << int32(*playerQuestReward.Honor);

    return data;
}

ByteBuffer& operator<<(ByteBuffer& data, LfgPlayerDungeonInfo const& playerDungeonInfo)
{
    data << uint32(playerDungeonInfo.Slot);
    data << int32(playerDungeonInfo.CompletionQuantity);
    data << int32(playerDungeonInfo.CompletionLimit);
    data << int32(playerDungeonInfo.CompletionCurrencyID);
    data << int32(playerDungeonInfo.SpecificQuantity);
    data << int32(playerDungeonInfo.SpecificLimit);
    data << int32(playerDungeonInfo.OverallQuantity);
    data << int32(playerDungeonInfo.OverallLimit);
    data << int32(playerDungeonInfo.PurseWeeklyQuantity);
    data << int32(playerDungeonInfo.PurseWeeklyLimit);
    data << int32(playerDungeonInfo.PurseQuantity);
    data << int32(playerDungeonInfo.PurseLimit);
    data << int32(playerDungeonInfo.Quantity);
    data << uint32(playerDungeonInfo.CompletedMask);
    data << uint32(playerDungeonInfo.EncounterMask);
    data << uint32(playerDungeonInfo.ShortageReward.size());
    data.WriteBit(playerDungeonInfo.FirstReward);
    data.WriteBit(playerDungeonInfo.ShortageEligible);
    data.FlushBits();

    data << playerDungeonInfo.Rewards;
    for (LfgPlayerQuestReward const& shortageReward : playerDungeonInfo.ShortageReward)
        data << shortageReward;

    return data;
}

WorldPacket const* LfgPlayerInfo::Write()
{
    _worldPacket << uint32(Dungeon.size());
    _worldPacket << BlackList;
    for (LfgPlayerDungeonInfo const& playerDungeonInfo : Dungeon)
        _worldPacket << playerDungeonInfo;

    return &_worldPacket;
}

WorldPacket const* LfgPartyInfo::Write()
{
    _worldPacket << uint32(Player.size());
    for (LFGBlackList const& player : Player)
        _worldPacket << player;

    return &_worldPacket;
}

WorldPacket const* LFGUpdateStatus::Write()
{
    _worldPacket << Ticket;
    _worldPacket << uint8(SubType);
    _worldPacket << uint8(Reason);
    _worldPacket << uint32(Slots.size());
    _worldPacket << uint8(RequestedRoles);
    _worldPacket << uint32(SuspendedPlayers.size());
    _worldPacket << uint32(QueueMapID);

    for (uint32 slot : Slots)
        _worldPacket << uint32(slot);

    for (ObjectGuid const& suspendedPlayer : SuspendedPlayers)
        _worldPacket << suspendedPlayer;

    _worldPacket.WriteBit(IsParty);
    _worldPacket.WriteBit(NotifyUI);
    _worldPacket.WriteBit(Joined);
    _worldPacket.WriteBit(LfgJoined);
    _worldPacket.WriteBit(Queued);
    _worldPacket.WriteBit(Brawl);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

WorldPacket const* RoleChosen::Write()
{
    _worldPacket << Player;
    _worldPacket << uint8(RoleMask);
    _worldPacket.WriteBit(Accepted);
    _worldPacket.FlushBits();

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, LFGRoleCheckUpdateMember const& lfgRoleCheckUpdateMember)
{
    data << lfgRoleCheckUpdateMember.Guid;
    data << uint8(lfgRoleCheckUpdateMember.RolesDesired);
    data << uint8(lfgRoleCheckUpdateMember.Level);
    data.WriteBit(lfgRoleCheckUpdateMember.RoleCheckComplete);
    data.FlushBits();

    return data;
}

WorldPacket const* LFGRoleCheckUpdate::Write()
{
    _worldPacket << uint8(PartyIndex);
    _worldPacket << uint8(RoleCheckStatus);
    _worldPacket << uint32(JoinSlots.size());
    _worldPacket << uint32(BgQueueIDs.size());
    _worldPacket << int32(GroupFinderActivityID);
    _worldPacket << uint32(Members.size());

    for (uint32 slot : JoinSlots)
        _worldPacket << uint32(slot);

    for (uint64 bgQueueID : BgQueueIDs)
        _worldPacket << uint64(bgQueueID);

    _worldPacket.WriteBit(IsBeginning);
    _worldPacket.WriteBit(IsRequeue);
    _worldPacket.FlushBits();

    for (LFGRoleCheckUpdateMember const& member : Members)
        _worldPacket << member;

    return &_worldPacket;
}

WorldPacket const* LFGJoinResult::Write()
{
    _worldPacket << Ticket;
    _worldPacket << int32(Result);
    _worldPacket << uint8(ResultDetail);
    _worldPacket << uint32(BlackList.size());
    _worldPacket << uint32(BlackListNames.size());

    for (LFGBlackList const& blackList : BlackList)
        _worldPacket << blackList;

    for (std::string const* str : BlackListNames)
        _worldPacket.WriteBits(str->length() + 1, 24);

    for (std::string const* str : BlackListNames)
        if (!str->empty())
            _worldPacket << *str;

    return &_worldPacket;
}

WorldPacket const* LFGQueueStatus::Write()
{
    _worldPacket << Ticket;
    _worldPacket << uint32(Slot);
    _worldPacket << uint32(AvgWaitTimeMe);
    _worldPacket << uint32(AvgWaitTime);

    for (uint32 i = 0; i < 3; ++i)
    {
        _worldPacket << uint32(AvgWaitTimeByRole[i]);
        _worldPacket << uint8(LastNeeded[i]);
    }
    _worldPacket << uint32(QueuedTime);

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, LFGPlayerRewards const& lfgPlayerRewards)
{
    data.WriteBit(lfgPlayerRewards.RewardItem.has_value());
    data.WriteBit(lfgPlayerRewards.RewardCurrency.has_value());
    if (lfgPlayerRewards.RewardItem)
        data << *lfgPlayerRewards.RewardItem;

    data << uint32(lfgPlayerRewards.Quantity);
    data << int32(lfgPlayerRewards.BonusQuantity);
    if (lfgPlayerRewards.RewardCurrency)
        data << int32(*lfgPlayerRewards.RewardCurrency);

    return data;
}

WorldPacket const* LFGPlayerReward::Write()
{
    _worldPacket << uint32(QueuedSlot);
    _worldPacket << uint32(ActualSlot);
    _worldPacket << int32(RewardMoney);
    _worldPacket << int32(AddedXP);
    _worldPacket << uint32(Rewards.size());

    for (LFGPlayerRewards const& reward : Rewards)
        _worldPacket << reward;

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, LfgBootInfo const& lfgBootInfo)
{
    data << Bits<1>(lfgBootInfo.VoteInProgress);
    data << Bits<1>(lfgBootInfo.VotePassed);
    data << Bits<1>(lfgBootInfo.MyVoteCompleted);
    data << Bits<1>(lfgBootInfo.MyVote);
    data << SizedString::BitsSize<9>(lfgBootInfo.Reason);
    data << lfgBootInfo.Target;
    data << uint32(lfgBootInfo.TotalVotes);
    data << uint32(lfgBootInfo.BootVotes);
    data << int32(lfgBootInfo.TimeLeft);
    data << uint32(lfgBootInfo.VotesNeeded);
    data << SizedString::Data(lfgBootInfo.Reason);

    return data;
}

WorldPacket const* LfgBootPlayer::Write()
{
    _worldPacket << Info;

    return &_worldPacket;
}

ByteBuffer& operator<<(ByteBuffer& data, LFGProposalUpdatePlayer const& lfgProposalUpdatePlayer)
{
    data << uint8(lfgProposalUpdatePlayer.Roles);
    data.WriteBit(lfgProposalUpdatePlayer.Me);
    data.WriteBit(lfgProposalUpdatePlayer.SameParty);
    data.WriteBit(lfgProposalUpdatePlayer.MyParty);
    data.WriteBit(lfgProposalUpdatePlayer.Responded);
    data.WriteBit(lfgProposalUpdatePlayer.Accepted);
    data.FlushBits();

    return data;
}

WorldPacket const* LFGProposalUpdate::Write()
{
    _worldPacket << Ticket;
    _worldPacket << uint64(InstanceID);
    _worldPacket << uint32(ProposalID);
    _worldPacket << uint32(Slot);
    _worldPacket << int8(State);
    _worldPacket << uint32(CompletedMask);
    _worldPacket << uint32(EncounterMask);
    _worldPacket << uint32(Players.size());
    _worldPacket << uint8(PromisedShortageRolePriority);
    _worldPacket.WriteBit(ValidCompletedMask);
    _worldPacket.WriteBit(ProposalSilent);
    _worldPacket.WriteBit(FailedByMyParty);
    _worldPacket.FlushBits();

    for (LFGProposalUpdatePlayer const& player : Players)
        _worldPacket << player;

    return &_worldPacket;
}

WorldPacket const* LFGOfferContinue::Write()
{
    _worldPacket << uint32(Slot);

    return &_worldPacket;
}

WorldPacket const* LFGTeleportDenied::Write()
{
    _worldPacket.WriteBits(Reason, 4);
    _worldPacket.FlushBits();

    return &_worldPacket;
}
}
