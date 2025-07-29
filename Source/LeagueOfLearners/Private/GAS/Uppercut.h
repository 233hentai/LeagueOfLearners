// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/LOLGameplayAbility.h"
#include "Uppercut.generated.h"

/**
 * 
 */
UCLASS()
class UUppercut : public ULOLGameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* UppercutMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float SweepSphereRadius = 80.f;
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float UppercutLaunchSpeed = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	TSubclassOf<UGameplayEffect> LaunchDamageEffect;

	static FGameplayTag GetUppercutLaunchTag();

	UFUNCTION()
	void StartLaunching(FGameplayEventData EventData);

	UFUNCTION()
	void HandleComboChangeEvent(FGameplayEventData EventData);
	FName NextComboName;

};
