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

public:
	ULOLGameplayAbility();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
	class UAnimInstance* GetOwnerAnimInstance() const;
	TArray<FHitResult> GetHitResultFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SweepSphereRadius = 30.f, ETeamAttitude::Type TargetTeam=ETeamAttitude::Hostile,bool bDrawDebug = false,bool bIgnoreSelf=true) const;

	UFUNCTION()
	FORCEINLINE bool ShouldDrawDebug() const { return bShouldDrawDebug; };

	void PushSelf(const FVector& PushVelocity);
	void PushTarget(AActor* Target,const FVector& PushVelocity);
	void PushTargets(const TArray<AActor*>& Targets, const FVector& PushVelocity);
	void PushTargets(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& PushVelocity);
	ACharacter* GetOwningAvatarActor();
	void ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult,TSubclassOf<UGameplayEffect> GameplayEffect,int level=1);
	void PlayMontageLocally(UAnimMontage* MontageToPlay);
	void StopMontageAfterCurrentSection(UAnimMontage* MontageToStop);
	FGenericTeamId GetOwnerTeamId() const;
	AActor* GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const;
	bool IsActorTeamAttitudeIs(const AActor* OtherActor, ETeamAttitude::Type TeamAttitude) const;
	void SendLocalGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bShouldDrawDebug=false;

	UPROPERTY()
	class ACharacter* AvatarCharacter;
};
