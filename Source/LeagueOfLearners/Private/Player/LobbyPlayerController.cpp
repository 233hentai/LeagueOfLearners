// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LobbyPlayerController.h"
#include "Framework/LOLGameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyPlayerController::Server_RequestSelectionChange_Implementation(uint8 NewSlotId)
{
	if (!GetWorld()) return;
	ALOLGameStateBase* LOLGameStateBase = GetWorld()->GetGameState<ALOLGameStateBase>();
	if (!LOLGameStateBase) return;
	LOLGameStateBase->RequestPlayerSelectionChange(GetPlayerState<APlayerState>(), NewSlotId);
}

bool ALobbyPlayerController::Server_RequestSelectionChange_Validate(uint8 NewSlotId)
{
	return true;
}