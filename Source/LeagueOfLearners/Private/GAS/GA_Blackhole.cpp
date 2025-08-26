// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_Blackhole.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "GAS/TargetActor_GroundPick.h"
#include "GAS/TA_Blackhole.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/LOLAbilitySystemStatics.h"

void UGA_Blackhole::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)) {
		K2_EndAbility();
		return;
	}
	PlayCastBlckholeMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);
	PlayCastBlckholeMontageTask->OnBlendOut.AddDynamic(this, &UGA_Blackhole::K2_EndAbility);
	PlayCastBlckholeMontageTask->OnCancelled.AddDynamic(this, &UGA_Blackhole::K2_EndAbility);
	PlayCastBlckholeMontageTask->OnInterrupted.AddDynamic(this, &UGA_Blackhole::K2_EndAbility);
	PlayCastBlckholeMontageTask->OnCompleted.AddDynamic(this, &UGA_Blackhole::K2_EndAbility);
	PlayCastBlckholeMontageTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* WaitPlacementTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	WaitPlacementTask->ValidData.AddDynamic(this, &UGA_Blackhole::PlaceBlackhole);
	WaitPlacementTask->Cancelled.AddDynamic(this, &UGA_Blackhole::PlacementCancelled);
	WaitPlacementTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitPlacementTask->BeginSpawningActor(this, TargetActorClass, TargetActor);
	ATargetActor_GroundPick* GroundPickTargetActor = Cast<ATargetActor_GroundPick>(TargetActor);
	if (GroundPickTargetActor) {
		GroundPickTargetActor->SetShouldDrawDebug(ShouldDrawDebug());
		GroundPickTargetActor->SetTargetAreaRadius(TargetAreaRadius);
		GroundPickTargetActor->SetTargetTraceRange(TargetTraceRange);
	}
	WaitPlacementTask->FinishSpawningActor(this, TargetActor);
	AddAimEffect();
}

void UGA_Blackhole::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	RemoveAimEffect();
	RemoveFocusEffect();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Blackhole::PlaceBlackhole(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (!K2_CommitAbility()) {
		K2_EndAbility();
		return;
	}
	RemoveAimEffect();
	AddFocusEffect();
	if (PlayCastBlckholeMontageTask) {
		PlayCastBlckholeMontageTask->OnBlendOut.RemoveAll(this);
		PlayCastBlckholeMontageTask->OnCancelled.RemoveAll(this);
		PlayCastBlckholeMontageTask->OnInterrupted.RemoveAll(this);
		PlayCastBlckholeMontageTask->OnCompleted.RemoveAll(this);
	}
	if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo)) {
		UAbilityTask_PlayMontageAndWait* PlayHoldBlackholeMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, HoldBlackholeMontage);
		PlayHoldBlackholeMontage->OnBlendOut.AddDynamic(this, &UGA_Blackhole::K2_EndAbility);
		PlayHoldBlackholeMontage->OnCancelled.AddDynamic(this, &UGA_Blackhole::K2_EndAbility);
		PlayHoldBlackholeMontage->OnInterrupted.AddDynamic(this, &UGA_Blackhole::K2_EndAbility);
		PlayHoldBlackholeMontage->OnCompleted.AddDynamic(this, &UGA_Blackhole::K2_EndAbility);
		PlayHoldBlackholeMontage->ReadyForActivation();
	}
	BlackholeTargetingTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, BlackholeTargetActorClass);
	BlackholeTargetingTask->ValidData.AddDynamic(this, &UGA_Blackhole::FinalTargetsReceived);
	BlackholeTargetingTask->Cancelled.AddDynamic(this, &UGA_Blackhole::FinalTargetsReceived);
	BlackholeTargetingTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	BlackholeTargetingTask->BeginSpawningActor(this, BlackholeTargetActorClass, TargetActor);
	ATA_Blackhole* BlackholeTargetActor = Cast<ATA_Blackhole>(TargetActor);
	if (BlackholeTargetActor) {
		BlackholeTargetActor->ConfigureBlackhole(TargetAreaRadius, BlackholePullSpeed, BlackholeDuration, GetOwnerTeamId());
	}
	BlackholeTargetingTask->FinishSpawningActor(this, TargetActor);
	if (BlackholeTargetActor) {
		BlackholeTargetActor->SetActorLocation(UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint);
	}
}

void UGA_Blackhole::PlacementCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	K2_EndAbility();
}

void UGA_Blackhole::FinalTargetsReceived(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (K2_HasAuthority()) {
		BP_ApplyGameplayEffectToTarget(TargetDataHandle, FinalBlastDamageEfffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		FVector BlastCenter = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint;
		PushTargetsFromLocation(TargetDataHandle, BlastCenter, BlastPushSpeed);

		UAbilityTask_PlayMontageAndWait* PlayFinalBlastMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FinalBlastMontage);
		PlayFinalBlastMontage->OnBlendOut.AddDynamic(this, &UGA_Blackhole::K2_EndAbility);
		PlayFinalBlastMontage->OnCancelled.AddDynamic(this, &UGA_Blackhole::K2_EndAbility);
		PlayFinalBlastMontage->OnInterrupted.AddDynamic(this, &UGA_Blackhole::K2_EndAbility);
		PlayFinalBlastMontage->OnCompleted.AddDynamic(this, &UGA_Blackhole::K2_EndAbility);
		PlayFinalBlastMontage->ReadyForActivation();
	}
	else {
		PlayMontageLocally(FinalBlastMontage);
	}

	FGameplayCueParameters FinalBlastCueParams;
	FinalBlastCueParams.Location= UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 1).ImpactPoint;
	FinalBlastCueParams.RawMagnitude = TargetAreaRadius;
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(FinalBlastCueTag, FinalBlastCueParams);
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ULOLAbilitySystemStatics::GetCameraShakeCueTag(), FinalBlastCueParams);
}

void UGA_Blackhole::AddAimEffect()
{
	AimEffectHandle = BP_ApplyGameplayEffectToOwner(AimEfffect);
}

void UGA_Blackhole::RemoveAimEffect()
{
	if (AimEffectHandle.IsValid()) {
		BP_RemoveGameplayEffectFromOwnerWithHandle(AimEffectHandle);
	}
}

void UGA_Blackhole::AddFocusEffect()
{
	FocusEffectHandle = BP_ApplyGameplayEffectToOwner(FocusEfffect);
}

void UGA_Blackhole::RemoveFocusEffect()
{
	if (FocusEffectHandle.IsValid()) {
		BP_RemoveGameplayEffectFromOwnerWithHandle(FocusEffectHandle);
	}
}
