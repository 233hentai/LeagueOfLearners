// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/LOLGameplayAbility.h"
#include "GAS/LOLGameplayAbilityTypes.h"
#include "Uppercut.generated.h"

/**
 * 
 */
UCLASS()
class UUppercut : public ULOLGameplayAbility
{
	GENERATED_BODY()
	
public:
	UUppercut();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TMap<FName, FGenericDamageEffectDef> ComboDamageMap;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* UppercutMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float SweepSphereRadius = 80.f;
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float UppercutLaunchSpeed = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float UppercutHoldSpeed = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	TSubclassOf<UGameplayEffect> LaunchDamageEffect;

	const FGenericDamageEffectDef* GetDamageEffectDefForCurrentCombo() const;
	static FGameplayTag GetUppercutLaunchTag();

	UFUNCTION()
	void StartLaunching(FGameplayEventData EventData);

	UFUNCTION()
	void HandleComboChangeEvent(FGameplayEventData EventData);
	UFUNCTION()
	void HandleComboCommitEvent(FGameplayEventData EventData);
	UFUNCTION()
	void HandleComboDamageEvent(FGameplayEventData EventData);

	FName NextComboName;

};
