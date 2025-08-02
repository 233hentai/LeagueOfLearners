// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_GroundPick.generated.h"

/**
 * 
 */
UCLASS()
class ATargetActor_GroundPick : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
public:
	ATargetActor_GroundPick();
	void SetTargetAreaRadius(float NewRadius);
	virtual void ConfirmTargetingAndContinue() override;
	void SetTargetOptions(bool bTargetFriend, bool bTargetEnemy = true);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceRange = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 300.f;
	bool bShouldTargetEnemy = true;
	bool bShouldTargetFriend = false;

	virtual void Tick(float DeltaTime) override;
	FVector GetTargetPoint() const;

};
