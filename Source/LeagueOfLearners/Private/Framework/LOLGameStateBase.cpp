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


void ALOLGameStateBase::SetCharacterSelected(const APlayerState* RequestingPlayer, const UPA_HeroDefinition* SelectedCharacter)
{
	if (IsDefinitionSelected(SelectedCharacter)) return;
	FPlayerSelection* FoundPlayerSelection = PlayerSelectionArray.FindByPredicate(
		[&](const FPlayerSelection& PlayerSelection)
		{
			return PlayerSelection.IsForPlayer(RequestingPlayer);
		}
	);
	if (FoundPlayerSelection) {
		FoundPlayerSelection->SetHeroDefinition(SelectedCharacter);
		OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
	}
}

bool ALOLGameStateBase::IsSlotOccupied(uint8 SlotId) const
{
	for (const FPlayerSelection& PlayerSelection : PlayerSelectionArray) {
		if (PlayerSelection.GetPlayerSlot() == SlotId) return true;
	}
	return false;
}

bool ALOLGameStateBase::IsDefinitionSelected(const UPA_HeroDefinition* Definition) const
{
	const FPlayerSelection* FoundPlayerSelection = PlayerSelectionArray.FindByPredicate(
		[&](const FPlayerSelection& PlayerSelection)
		{
			return PlayerSelection.GetHeroDefinition() == Definition;
		}
	);
	return FoundPlayerSelection != nullptr;
}


const TArray<FPlayerSelection>& ALOLGameStateBase::GetPlayerSelection() const
{
	return PlayerSelectionArray;
}

bool ALOLGameStateBase::CanStartHeroSelection() const
{
	return PlayerSelectionArray.Num() == PlayerArray.Num();
}

void ALOLGameStateBase::SetCharacterDeselected(const UPA_HeroDefinition* DefinitionToDeselect)
{
	if (!DefinitionToDeselect) return;
	FPlayerSelection* FoundPlayerSelection = PlayerSelectionArray.FindByPredicate(
		[&](const FPlayerSelection& PlayerSelection)
		{
			return PlayerSelection.GetHeroDefinition() == DefinitionToDeselect;
		}
	);
	if (FoundPlayerSelection) {
		FoundPlayerSelection->SetHeroDefinition(nullptr);
		OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
	}
}

bool ALOLGameStateBase::CanStartMatch() const
{
	for (const FPlayerSelection& PlayerSelection : PlayerSelectionArray) {
		if (PlayerSelection.GetHeroDefinition() == nullptr) {
			return false;
		}
	}
	return true;
}


void ALOLGameStateBase::OnRep_PlayerSelectionArray()
{
	OnPlayerSelectionUpdated.Broadcast(PlayerSelectionArray);
}
