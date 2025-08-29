// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LobbyPlayerController.h"
#include "Framework/LOLGameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Framework/LOLGameInstance.h"

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

void ALobbyPlayerController::Server_StartHeroSelection_Implementation()
{
	if (!HasAuthority() || !GetWorld()) return;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator) {
		ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(*Iterator);
		if (LobbyPlayerController) {
			LobbyPlayerController->Client_StartHeroSelection();
		}
	}
}

bool ALobbyPlayerController::Server_StartHeroSelection_Validate()
{
	return true;
}

void ALobbyPlayerController::Client_StartHeroSelection_Implementation()
{
	OnSwitchToHeroSelection.ExecuteIfBound();
}

ALobbyPlayerController::ALobbyPlayerController()
{
	bAutoManageActiveCameraTarget = false;
}

void ALobbyPlayerController::Server_RequestStartMatch_Implementation()
{
	ULOLGameInstance* LOLGameInstance = GetGameInstance<ULOLGameInstance>();
	if (LOLGameInstance) {
		LOLGameInstance->StartMatch();
	}
}

bool ALobbyPlayerController::Server_RequestStartMatch_Validate()
{
	return true;
}