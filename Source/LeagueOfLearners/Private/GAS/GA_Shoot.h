// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/LOLGameplayAbility.h"
#include "GA_Shoot.generated.h"

/**
 * 
 */
UCLASS()
class UGA_Shoot : public ULOLGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_Shoot();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Animation")
	UAnimMontage* ShootMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Shoot")
	TSubclassOf<class AProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category = "Shoot")
	TSubclassOf<UGameplayEffect> ProjectileHitEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Shoot")
	float ShootProjectileSpeed = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Shoot")
	float ShootProjectileRange = 4000.f;

	UPROPERTY()
	AActor* AimTarget;
	UPROPERTY()
	UAbilitySystemComponent* AimTargetASC;
	UPROPERTY(EditDefaultsOnly, Category = "Target")
	float AimTargetCheckInterval = 0.1f;
	FTimerHandle AimTargetCheckTimerHandle;


	static FGameplayTag GetShootTag();

	UFUNCTION()
	void StartShooting(FGameplayEventData EventData);
	UFUNCTION()
	void StopShooting(FGameplayEventData EventData);
	UFUNCTION()
	void ShootProjectile(FGameplayEventData EventData);

	AActor* GetAimTargetIfValid() const;
	void FindAimTarget();
	void StartAimTargetCheckTimer();
	void StopAimTargetCheckTimer();
	bool HasValidTarget() const;
	bool IsTargetInRange() const;
	void TargetDeadTagUpdated(const FGameplayTag Tag, int NewCount);
};
