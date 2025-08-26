// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Player/PlayerInfoTypes.h"
#include "LOLGameStateBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSelectionUpdated,const TArray<FPlayerSelection>& /*NewPlayerSelection*/);
/**
 * 
 */
UCLASS()
class ALOLGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	FOnPlayerSelectionUpdated OnPlayerSelectionUpdated;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	void RequestPlayerSelectionChange(const APlayerState* RequestingPlayer, uint8 DesiredSlot);
	bool IsSlotOccupied(uint8 SlotId) const;
	const TArray<FPlayerSelection>& GetPlayerSelection() const;
private:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerSelectionArray)
	TArray<FPlayerSelection> PlayerSelectionArray;

	UFUNCTION()
	void OnRep_PlayerSelectionArray();
};
