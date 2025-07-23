// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GenericTeamAgentInterface.h"
#include "LOLGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ULOLGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
protected:
	class UAnimInstance* GetOwnerAnimInstance() const;
	TArray<FHitResult> GetHitResultFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SweepSphereRadius = 30.f, ETeamAttitude::Type TargetTeam=ETeamAttitude::Hostile,bool bDrawDebug = false,bool bIgnoreSelf=true) const;
};
