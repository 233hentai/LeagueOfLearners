// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LOLGameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "Framework/StormCore.h"
#include "Player/LOLPlayerController.h"

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

FGenericTeamId ALOLGameModeBase::GetTeamIDForPlayer(const APlayerController* PlayerController) const
{
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
