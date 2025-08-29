// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LOLGameInstance.h"


void ULOLGameInstance::StartMatch()
{
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer|| GetWorld()->GetNetMode() == ENetMode::NM_ListenServer) {
		LoadLevelAndListen(GameLevel);
	}
}

void ULOLGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
	const FName LevelURL = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));
	if (LevelURL != "") {
		GetWorld()->ServerTravel(LevelURL.ToString()+"?listen");
	}
}
