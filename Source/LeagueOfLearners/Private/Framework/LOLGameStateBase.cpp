// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LOLGameStateBase.h"
#include "Net/UnrealNetwork.h"

void ALOLGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ALOLGameStateBase, PlayerSelectionArray, COND_None, REPNOTIFY_Always);
}

void ALOLGameStateBase::RequestPlayerSelectionChange(const APlayerState* RequestingPlayer, uint8 DesiredSlot)
{
	if (!HasAuthority() || IsSlotOccupied(DesiredSlot)) return;
	FPlayerSelection* PlayerSelectionPtr = PlayerSelectionArray.FindByPredicate([&](const FPlayerSelection& PlayerSelection)
		{
			return PlayerSelection.IsForPlayer(RequestingPlayer);
		});
	if (PlayerSelectionPtr) {
		PlayerSelectionPtr->SetSlot(DesiredSlot);
	}
	else {
		PlayerSelectionArray.Add(FPlayerSelection(DesiredSlot, RequestingPlayer));
	}
	OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
}

bool ALOLGameStateBase::IsSlotOccupied(uint8 SlotId) const
{
	for (const FPlayerSelection& PlayerSelection : PlayerSelectionArray) {
		if (PlayerSelection.GetPlayerSlot() == SlotId) return true;
	}
	return false;
}

const TArray<FPlayerSelection>& ALOLGameStateBase::GetPlayerSelection() const
{
	return PlayerSelectionArray;
}

void ALOLGameStateBase::OnRep_PlayerSelectionArray()
{
	OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
}
