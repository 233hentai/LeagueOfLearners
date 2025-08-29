// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerInfoTypes.h"
#include "GameFramework/PlayerState.h"
#include "Network/LOLNetStatics.h"

//FPlayerSelection::FPlayerSelection() :Slot{ GetInvalidSlot() }, PlayerUniqueId{ FUniqueNetIdRepl::Invalid() }, PlayerNickName{}
FPlayerSelection::FPlayerSelection() :Slot{ GetInvalidSlot() }, PlayerUniqueId{ FUniqueNetIdRepl::Invalid() }, PlayerNickName{}, HeroDefinition(nullptr)
{
}

//FPlayerSelection::FPlayerSelection(uint8 InSlot, const APlayerState* InPlayerState) :Slot{InSlot}
FPlayerSelection::FPlayerSelection(uint8 InSlot, const APlayerState* InPlayerState) :Slot{InSlot}, HeroDefinition(nullptr)
{
	if (InPlayerState) {
		PlayerUniqueId = InPlayerState->GetUniqueId();
		PlayerNickName = InPlayerState->GetPlayerName();
	}
}


bool FPlayerSelection::IsForPlayer(const APlayerState* PlayerState) const
{
	if (!PlayerState) return false;

#if WITH_EDITOR
	return PlayerState->GetPlayerName() == PlayerNickName;
#else
	return PlayerState->GetUniqueId() == GetPlayerUniqueId();
#endif
}

bool FPlayerSelection::IsValid() const
{
#if WITH_EDITOR
	if (Slot == GetInvalidSlot()) return false;
	if (Slot >= ULOLNetStatics::GetPlayerCountPerTeam() * 2) return false;
	return true;
#else
	if (!PlayerUniqueId.IsValid()) return false;
	if (Slot == GetInvalidSlot()) return false;
	if (Slot >= ULOLNetStatics::GetPlayerCountPerTeam() * 2) return false;
	return true;
#endif
}

//bool FPlayerSelection::IsValid() const
//{
//#if WITH_EDITOR
//	return true;
//#else
//	if (!PlayerUniqueId.IsValid()) return false;
//	if (Slot == GetInvalidSlot()) return false;
//	if (Slot >= ULOLNetStatics::GetPlayerCountPerTeam() * 2) return false;
//	return true;
//#endif
//}

uint8 FPlayerSelection::GetInvalidSlot()
{
	return 255;
}
