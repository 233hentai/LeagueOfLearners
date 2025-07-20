// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
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
};
