// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LOLPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Framework/LOLGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Character/PA_HeroDefinition.h"
#include "Character/LOLCharacter.h"
#include "Network/LOLNetStatics.h"

ALOLPlayerState::ALOLPlayerState()
{
	bReplicates = true;
	NetUpdateFrequency = 100.f;
}

void ALOLPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALOLPlayerState, PlayerSelection);
}

void ALOLPlayerState::BeginPlay()
{
	Super::BeginPlay();
	LOLGameState = Cast<ALOLGameStateBase>(UGameplayStatics::GetGameState(this));
	if (LOLGameState) {
		LOLGameState->OnPlayerSelectionUpdated.AddUObject(this, &ALOLPlayerState::PlayerSelectionUpdated);
	}
}

void ALOLPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	ALOLPlayerState* NewPlayerState = Cast<ALOLPlayerState>(PlayerState);
	if (NewPlayerState) {
		NewPlayerState->PlayerSelection = PlayerSelection;
	}
}

TSubclassOf<APawn> ALOLPlayerState::GetSelectedPawnClass() const
{
	if (PlayerSelection.GetHeroDefinition()) {
		return PlayerSelection.GetHeroDefinition()->LoadCharacterClass();
	}
	return nullptr;
}

FGenericTeamId ALOLPlayerState::GetTeamIdBasedOnSlot() const
{
	return PlayerSelection.GetPlayerSlot() < ULOLNetStatics::GetPlayerCountPerTeam() ? FGenericTeamId{ 0 } : FGenericTeamId{ 1 };
}


void ALOLPlayerState::Server_SetSelectedHeroDefinition_Implementation(const UPA_HeroDefinition* NewDefinition)
{
	if (!LOLGameState) return;
	if (!NewDefinition) return;
	if (LOLGameState->IsDefinitionSelected(NewDefinition)) return;
	if (PlayerSelection.GetHeroDefinition()) {
		LOLGameState->SetCharacterDeselected(PlayerSelection.GetHeroDefinition());
	}

	PlayerSelection.SetHeroDefinition(NewDefinition);
	LOLGameState->SetCharacterSelected(this, NewDefinition);
}

bool ALOLPlayerState::Server_SetSelectedHeroDefinition_Validate(const UPA_HeroDefinition* NewDefinition)
{
	return true;
}

void ALOLPlayerState::PlayerSelectionUpdated(const TArray<FPlayerSelection>& NewPlayerSelections)
{
	for (const FPlayerSelection& NewPlayerSelection : NewPlayerSelections) {
		if (NewPlayerSelection.IsForPlayer(this)) {
			PlayerSelection = NewPlayerSelection;
		}
	}
}
