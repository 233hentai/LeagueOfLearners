// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/LOLGameplayAbility.h"
#include "GA_Blackhole.generated.h"

/**
 * 
 */
UCLASS()
class UGA_Blackhole : public ULOLGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* TargetingMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* HoldBlackholeMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FinalBlastMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ATargetActor_GroundPick> TargetActorClass;
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceRange = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float BlackholePullSpeed = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	float BlastPushSpeed = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float BlackholeDuration = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<UGameplayEffect> AimEfffect;
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<UGameplayEffect> FocusEfffect;
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<class ATA_Blackhole> BlackholeTargetActorClass;
	UPROPERTY()
	class UAbilityTask_PlayMontageAndWait* PlayCastBlckholeMontageTask;
	UPROPERTY()
	class UAbilityTask_WaitTargetData* BlackholeTargetingTask;
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TSubclassOf<UGameplayEffect> FinalBlastDamageEfffect;
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	FGameplayTag FinalBlastCueTag;

	FActiveGameplayEffectHandle AimEffectHandle;
	FActiveGameplayEffectHandle FocusEffectHandle;

	UFUNCTION()
	void PlaceBlackhole(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	UFUNCTION()
	void PlacementCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle);
	UFUNCTION()
	void FinalTargetsReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	void AddAimEffect();
	void RemoveAimEffect();

	void AddFocusEffect();
	void RemoveFocusEffect();
};
