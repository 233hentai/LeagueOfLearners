// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_Lazer.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/LOLAttributeSet.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GAS/TargetActor_Line.h"

void UGA_Lazer::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility()||!LazerMontage) {
		K2_EndAbility();
		return;
	}
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)) {
		UAbilityTask_PlayMontageAndWait* PlayerLazerMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, LazerMontage);
		PlayerLazerMontage->OnBlendOut.AddDynamic(this, &UGA_Lazer::K2_EndAbility);
		PlayerLazerMontage->OnCancelled.AddDynamic(this, &UGA_Lazer::K2_EndAbility);
		PlayerLazerMontage->OnInterrupted.AddDynamic(this, &UGA_Lazer::K2_EndAbility);
		PlayerLazerMontage->OnCompleted.AddDynamic(this, &UGA_Lazer::K2_EndAbility);
		PlayerLazerMontage->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitShootEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetShootTag());
		WaitShootEvent->EventReceived.AddDynamic(this, &UGA_Lazer::ShootLazer);
		WaitShootEvent->ReadyForActivation();

		UAbilityTask_WaitCancel* WaitCancel = UAbilityTask_WaitCancel::WaitCancel(this);
		WaitCancel->OnCancel.AddDynamic(this, &UGA_Lazer::K2_EndAbility);
		WaitCancel->ReadyForActivation();
	}
}

void UGA_Lazer::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
	if (OwnerASC && ConsumtionEffectHandle.IsValid()) {
		OwnerASC->RemoveActiveGameplayEffect(ConsumtionEffectHandle);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FGameplayTag UGA_Lazer::GetShootTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.Lazer.Shoot");
}

void UGA_Lazer::ShootLazer(FGameplayEventData EventData)
{
	if (K2_HasAuthority()) {
		ConsumtionEffectHandle = BP_ApplyGameplayEffectToOwner(ConsumtionEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
		UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
		if (OwnerASC) {
			OwnerASC->GetGameplayAttributeValueChangeDelegate(ULOLAttributeSet::GetManaAttribute()).AddUObject(this, &UGA_Lazer::ManaUpdated);
		}
	}
	UAbilityTask_WaitTargetData* WaitTargetTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::CustomMulti, LazerTargetActorClass);
	WaitTargetTask->ValidData.AddDynamic(this, &UGA_Lazer::TargetReceived);
	WaitTargetTask->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetTask->BeginSpawningActor(this, LazerTargetActorClass, TargetActor);
	ATargetActor_Line* LineTargetActor = Cast<ATargetActor_Line>(TargetActor);
	if (LineTargetActor) {
		LineTargetActor->ConfigureTargetSetting(TargetRange, DetectionCylinderRadius, TargetingInterval, GetOwnerTeamId(), ShouldDrawDebug());
	}

	WaitTargetTask->FinishSpawningActor(this, TargetActor);

	if (LineTargetActor) {
		LineTargetActor->AttachToComponent(GetOwningComponentFromActorInfo(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TargetActorAttachSocketName);
	}
}

void UGA_Lazer::ManaUpdated(const FOnAttributeChangeData& ChangeData)
{
	UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
	if (OwnerASC&&!OwnerASC->CanApplyAttributeModifiers(ConsumtionEffect.GetDefaultObject(),GetAbilityLevel(CurrentSpecHandle,CurrentActorInfo),MakeEffectContext(CurrentSpecHandle,CurrentActorInfo))) {
		K2_EndAbility();
	}
}

void UGA_Lazer::TargetReceived(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	if (K2_HasAuthority()) {
		BP_ApplyGameplayEffectToTarget(DataHandle, DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
	}
	PushTargets(DataHandle, GetAvatarActorFromActorInfo()->GetActorForwardVector() * PushSpeed);
}
