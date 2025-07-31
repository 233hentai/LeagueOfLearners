// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LOLPlayerController.h"
#include "Player/LOLPlayerCharacter.h"
#include "Widgets/GameplayWidget.h"
#include "Net/UnrealNetwork.h"

void ALOLPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	LOLPlayerCharacter = Cast<ALOLPlayerCharacter>(NewPawn);
	if (LOLPlayerCharacter) {
		LOLPlayerCharacter->ServerSideInit();
		LOLPlayerCharacter->SetGenericTeamId(TeamID);
	}
}

void ALOLPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	LOLPlayerCharacter = Cast<ALOLPlayerCharacter>(NewPawn);
	if (LOLPlayerCharacter) {
		LOLPlayerCharacter->ClientSideInit();
		SpawnGameplayWidget();
	}

}

void ALOLPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId ALOLPlayerController::GetGenericTeamId() const
{
	return TeamID;
}

void ALOLPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALOLPlayerController,TeamID);
}

void ALOLPlayerController::SpawnGameplayWidget()
{
	if (!IsLocalPlayerController()) return;
	GameplayWidget = CreateWidget<UGameplayWidget>(this,GameplayWidgetClass);
	if (GameplayWidget) {
		GameplayWidget->AddToViewport();
		GameplayWidget->ConfigureAbilities(LOLPlayerCharacter->GetAbilities());
	}
}
