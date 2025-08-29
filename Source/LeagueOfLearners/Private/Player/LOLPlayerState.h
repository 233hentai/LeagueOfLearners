// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Player/PlayerInfoTypes.h"
#include "GenericTeamAgentInterface.h"
#include "LOLPlayerState.generated.h"

class UPA_HeroDefinition;
/**
 * 
 */
UCLASS()
class ALOLPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ALOLPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void CopyProperties(APlayerState* PlayerState) override;

	TSubclassOf<APawn> GetSelectedPawnClass() const;
	FGenericTeamId GetTeamIdBasedOnSlot() const;

	UFUNCTION(Server,Reliable,WithValidation)
	void Server_SetSelectedHeroDefinition(const UPA_HeroDefinition* NewDefinition);


private:
	UPROPERTY(Replicated)
	FPlayerSelection PlayerSelection;
	UPROPERTY()
	class ALOLGameStateBase* LOLGameState;

	void PlayerSelectionUpdated(const TArray<FPlayerSelection>& NewPlayerSelections);
};
