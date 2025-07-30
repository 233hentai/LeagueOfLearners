// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "LOLGameplayAbilityTypes.generated.h"

UENUM(BlueprintType)
enum class ELOLAbilityInputID :uint8
{
	None						UMETA(DisplayName = "None"),
	BasicAttack					UMETA(DisplayName = "Basic Attack"),
	Ability1					UMETA(DisplayName = "Ability 1"),
	Ability2					UMETA(DisplayName = "Ability 2"),
	Ability3					UMETA(DisplayName = "Ability 3"),
	Ability4					UMETA(DisplayName = "Ability 4"),
	Ability5					UMETA(DisplayName = "Ability 5"),
	Ability6					UMETA(DisplayName = "Ability 6"),
	Confirm						UMETA(DisplayName = "Confirm"),
	Cancel						UMETA(DisplayName = "Cancel")
};

USTRUCT(BlueprintType)
struct FGenericDamageEffectDef {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere)
	FVector PushVelocity;
}; 