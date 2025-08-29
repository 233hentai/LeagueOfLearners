// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LOLGameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Framework/StormCore.h"
#include "Player/LOLPlayerController.h"
#include "Player/LOLPlayerState.h"
#include "GameFramework/Controller.h"

APlayerController* ALOLGameModeBase::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
    APlayerController* NewPlayerController = Super::SpawnPlayerController(InRemoteRole, Options);
    IGenericTeamAgentInterface* NewTeamAgentInterface = Cast<IGenericTeamAgentInterface>(NewPlayerController);
    FGenericTeamId TeamID = GetTeamIDForPlayer(NewPlayerController);
    if (NewTeamAgentInterface) {
        NewTeamAgentInterface->SetGenericTeamId(TeamID);
    }

    NewPlayerController->StartSpot = FindNextStartSpotForTeam(TeamID);

    return NewPlayerController;
}

void ALOLGameModeBase::StartPlay()
{
    Super::StartPlay();
    AStormCore* StormCore = GetStormCore();
    if (StormCore) {
        StormCore->OnGoalReached.AddUObject(this, &ALOLGameModeBase::MatchFinished);
    }
}

UClass* ALOLGameModeBase::GetDefaultPawnClassForController_Implementation(AController* Controller)
{
    ALOLPlayerState* LOLPlayerState = Controller->GetPlayerState<ALOLPlayerState>();
    if (LOLPlayerState && LOLPlayerState->GetSelectedPawnClass()) {
        return LOLPlayerState->GetSelectedPawnClass();
    }
    return BackupPawn;
}

APawn* ALOLGameModeBase::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
    IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayer);
    FGenericTeamId TeamId = GetTeamIDForPlayer(NewPlayer);
    if (NewPlayerTeamInterface) {
        NewPlayerTeamInterface->SetGenericTeamId(TeamId);
    }
    StartSpot = FindNextStartSpotForTeam(TeamId);
    NewPlayer->StartSpot = StartSpot;
    return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}

FGenericTeamId ALOLGameModeBase::GetTeamIDForPlayer(const AController* InController) const
{
    ALOLPlayerState* LOLPlayerState = InController->GetPlayerState<ALOLPlayerState>();
    if (LOLPlayerState && LOLPlayerState->GetSelectedPawnClass()) {
        return LOLPlayerState->GetTeamIdBasedOnSlot();
    }
    static int PlayerCount = 0;
    ++PlayerCount;
    return FGenericTeamId(PlayerCount%2);
}

AActor* ALOLGameModeBase::FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const
{
    const FName* StartSpotTag = TeamStartSpotTagMap.Find(TeamID);
    if (!StartSpotTag) return nullptr;
    UWorld* World = GetWorld();
    for (TActorIterator<APlayerStart> Iterator(World); Iterator; ++Iterator) {
        if (Iterator->PlayerStartTag == *StartSpotTag) {
            Iterator->PlayerStartTag = FName("Taken");
            return *Iterator;
        }
    }
    return nullptr;
}

AStormCore* ALOLGameModeBase::GetStormCore() const
{
    UWorld* World = GetWorld();
    if (World) {
        for (TActorIterator<AStormCore> It(World); It; ++It) {
            return *It;
        }
    }
    return nullptr;
}

void ALOLGameModeBase::MatchFinished(AActor* ViewTarget, int WinnerTeam)
{
    UWorld* World = GetWorld();
    if (World) {
        for (TActorIterator<ALOLPlayerController> It(World); It; ++It) {
            It->MatchFinished(ViewTarget, WinnerTeam);
        }
    }
}
