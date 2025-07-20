// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS/LOLGameplayAbilityTypes.h"
#include "LOLAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class ULOLAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void ApplyInitialEffects();
	void GiveInitialAbilities();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<ELOLAbilityInputID,TSubclassOf<UGameplayAbility>> Abilities;
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<ELOLAbilityInputID,TSubclassOf<UGameplayAbility>> BasicAbilities;
};
