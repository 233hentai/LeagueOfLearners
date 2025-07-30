// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Uppercut.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GAS/GA_Combo.h"
#include "GameplayTagsManager.h"
#include "GAS/LOLAbilitySystemStatics.h"

UUppercut::UUppercut()
{
	BlockAbilitiesWithTag.AddTag(ULOLAbilitySystemStatics::GetBasicAttackAbilityTag());
}

void UUppercut::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility()) {
		K2_EndAbility();
		return;
	}
	if (HasAuthorityOrPredictionKey(ActorInfo,&ActivationInfo)) {
		UAbilityTask_PlayMontageAndWait* PlayUppercutMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,UppercutMontage);
		PlayUppercutMontageTask->OnBlendOut.AddDynamic(this,&UUppercut::K2_EndAbility);
		PlayUppercutMontageTask->OnCancelled.AddDynamic(this, &UUppercut::K2_EndAbility);
		PlayUppercutMontageTask->OnInterrupted.AddDynamic(this, &UUppercut::K2_EndAbility);
		PlayUppercutMontageTask->OnCompleted.AddDynamic(this, &UUppercut::K2_EndAbility);
		PlayUppercutMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitLaunchEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetUppercutLaunchTag());
		WaitLaunchEvent->EventReceived.AddDynamic(this, &UUppercut::StartLaunching);
		WaitLaunchEvent->ReadyForActivation();
	}
	NextComboName = NAME_None;
}

const FGenericDamageEffectDef* UUppercut::GetDamageEffectDefForCurrentCombo() const
{
	UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
	if (OwnerAnimInstance) {
		FName CurrentComboName = OwnerAnimInstance->Montage_GetCurrentSection(UppercutMontage);
		const FGenericDamageEffectDef* EffectDef = ComboDamageMap.Find(CurrentComboName);
		return EffectDef;
	}
	return nullptr;
}

FGameplayTag UUppercut::GetUppercutLaunchTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.Uppercut.Launch");
}

void UUppercut::StartLaunching(FGameplayEventData EventData)
{
	if (K2_HasAuthority()) {
		TArray<FHitResult> TargetHitResults = GetHitResultFromSweepLocationTargetData(EventData.TargetData, SweepSphereRadius, ETeamAttitude::Hostile, ShouldDrawDebug());
		PushTarget(GetAvatarActorFromActorInfo(),FVector::UpVector*UppercutLaunchSpeed);
		for (FHitResult& HitResult : TargetHitResults) {
			//UE_LOG(LogTemp, Warning, TEXT("Hit: %s"),*HitResult.GetActor()->GetName());
			PushTarget(HitResult.GetActor(), FVector::UpVector * UppercutLaunchSpeed);
			ApplyGameplayEffectToHitResultActor(HitResult,LaunchDamageEffect,GetAbilityLevel(CurrentSpecHandle,CurrentActorInfo));
		}
	}

	UAbilityTask_WaitGameplayEvent* WaitComboChangeEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,UGA_Combo::GetComboChangedEventTag(),nullptr,false,false);
	WaitComboChangeEvent->EventReceived.AddDynamic(this,&UUppercut::HandleComboChangeEvent);
	WaitComboChangeEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitComboCommitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,ULOLAbilitySystemStatics::GetBasicAttackInputPressedTag());
	WaitComboCommitEvent->EventReceived.AddDynamic(this, &UUppercut::HandleComboCommitEvent);
	WaitComboCommitEvent->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitComboDamageEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,UGA_Combo::GetComboTargetEventEndTag());
	WaitComboDamageEvent->EventReceived.AddDynamic(this, &UUppercut::HandleComboDamageEvent);
	WaitComboDamageEvent->ReadyForActivation();
}

void UUppercut::HandleComboChangeEvent(FGameplayEventData EventData)
{
	FGameplayTag EventTag = EventData.EventTag;
	if (EventTag == UGA_Combo::GetComboChangedEventEndTag()) {
		NextComboName = NAME_None;
		UE_LOG(LogTemp, Warning, TEXT("Next combo is cleared"));
		return;
	}

	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
	NextComboName = TagNames.Last();
	UE_LOG(LogTemp, Warning, TEXT("Next combo is %s"), *NextComboName.ToString());
}

void UUppercut::HandleComboCommitEvent(FGameplayEventData EventData)
{
	if (NextComboName == NAME_None) {
		return;
	}
	UAnimInstance* OwnerAnimInstace = GetOwnerAnimInstance();
	if (!OwnerAnimInstace) return;
	OwnerAnimInstace->Montage_SetNextSection(OwnerAnimInstace->Montage_GetCurrentSection(UppercutMontage), NextComboName, UppercutMontage);
}

void UUppercut::HandleComboDamageEvent(FGameplayEventData EventData)
{
	if (K2_HasAuthority()) {
		TArray<FHitResult> TargetHitResults = GetHitResultFromSweepLocationTargetData(EventData.TargetData, SweepSphereRadius, ETeamAttitude::Hostile, ShouldDrawDebug());
		PushTarget(GetAvatarActorFromActorInfo(), FVector::UpVector * UppercutHoldSpeed);
		const FGenericDamageEffectDef* EffectDef = GetDamageEffectDefForCurrentCombo();
		if (!EffectDef) return;
		for (FHitResult& HitResult : TargetHitResults) {
			FVector PushVelocity = GetAvatarActorFromActorInfo()->GetActorTransform().TransformVector(EffectDef->PushVelocity);
			PushTarget(HitResult.GetActor(), PushVelocity);
			ApplyGameplayEffectToHitResultActor(HitResult, EffectDef->DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		}
	}
}
