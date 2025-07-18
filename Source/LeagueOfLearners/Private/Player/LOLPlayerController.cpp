// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LOLPlayerController.h"
#include "Player/LOLPlayerCharacter.h"

void ALOLPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	LOLPlayerCharacter = Cast<ALOLPlayerCharacter>(NewPawn);
	if (LOLPlayerCharacter) {
		LOLPlayerCharacter->ServerSideInit();
	}
}

void ALOLPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	LOLPlayerCharacter = Cast<ALOLPlayerCharacter>(NewPawn);
	if (LOLPlayerCharacter) {
		LOLPlayerCharacter->ClientSideInit();
	}

}
