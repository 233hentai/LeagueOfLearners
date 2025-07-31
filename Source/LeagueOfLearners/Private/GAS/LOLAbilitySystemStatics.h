// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LOLAbilitySystemStatics.generated.h"

/**
 * 
 */
UCLASS()
class ULOLAbilitySystemStatics:public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static FGameplayTag GetBasicAttackAbilityTag();
	static FGameplayTag GetDeadStatTag();
	static FGameplayTag GetStunStatTag();
	static FGameplayTag GetBasicAttackInputPressedTag();

	static float GetStaticCooldownDurationForAbility(const class UGameplayAbility* Ability);
	static float GetStaticCostForAbility(const class UGameplayAbility* Ability);
};
