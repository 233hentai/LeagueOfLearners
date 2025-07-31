// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS/LOLGameplayAbilityTypes.h"
#include "GameplayEffectTypes.h"
#include "LOLAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class ULOLAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	ULOLAbilitySystemComponent();
	void ApplyInitialEffects();
	void GiveInitialAbilities();
	void ApplyFullStatEffect();
	const TMap<ELOLAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;

private:
	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> Effect,int Level=1);

	void HealthUpdated(const FOnAttributeChangeData& ChangeData);

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<ELOLAbilityInputID,TSubclassOf<UGameplayAbility>> Abilities;
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<ELOLAbilityInputID,TSubclassOf<UGameplayAbility>> BasicAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> FullStatEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> DeathEffect;

	
};
