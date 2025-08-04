// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_GroundBlast.h"
#include "GAS/LOLAbilitySystemStatics.h"
#include "GAS/TargetActor_GroundPick.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

UGA_GroundBlast::UGA_GroundBlast()
{
	ActivationOwnedTags.AddTag(ULOLAbilitySystemStatics::GetAimStatTag());
	BlockAbilitiesWithTag.AddTag(ULOLAbilitySystemStatics::GetBasicAttackAbilityTag());
}

void UGA_GroundBlast::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo)) return;

	UAbilityTask_PlayMontageAndWait* PlayGroundBlastAnimTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this,NAME_None,GroundBlastMontage);
	PlayGroundBlastAnimTask->OnBlendOut.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlastAnimTask->OnCancelled.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlastAnimTask->OnInterrupted.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlastAnimTask->OnCompleted.AddDynamic(this, &UGA_GroundBlast::K2_EndAbility);
	PlayGroundBlastAnimTask->ReadyForActivation();

	UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
	WaitTargetData->ValidData.AddDynamic(this, &UGA_GroundBlast::TargetConfirmed);
	WaitTargetData->Cancelled.AddDynamic(this, &UGA_GroundBlast::TargetCancelled);
	WaitTargetData->ReadyForActivation();

	AGameplayAbilityTargetActor* TargetActor;
	WaitTargetData->BeginSpawningActor(this,TargetActorClass,TargetActor);
	ATargetActor_GroundPick* GroundPickActor = Cast<ATargetActor_GroundPick>(TargetActor);
	if (GroundPickActor) {
		GroundPickActor->SetTargetAreaRadius(TargetAreaRadius);
		GroundPickActor->SetTargetTraceRange(TargetTraceRange);
		GroundPickActor->SetShouldDrawDebug(ShouldDrawDebug());
	}

	WaitTargetData->FinishSpawningActor(this,TargetActor);
}

void UGA_GroundBlast::TargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	/*TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
	for (AActor* Actor : TargetActors) {
		UE_LOG(LogTemp, Warning, TEXT("Target confirmed:%s"),*Actor->GetName());
	}*/
	BP_ApplyGameplayEffectToTarget(TargetDataHandle,DamageEffectDef.DamageEffect,GetAbilityLevel(CurrentSpecHandle,CurrentActorInfo));
	PushTargets(TargetDataHandle, DamageEffectDef.PushVelocity);

	FGameplayCueParameters BlastCueParams;
	BlastCueParams.Location = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle,1).ImpactPoint;
	BlastCueParams.RawMagnitude = TargetAreaRadius;
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(BlastGameplayCueTag, BlastCueParams);
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(ULOLAbilitySystemStatics::GetCameraShakeCueTag(), BlastCueParams);

	K2_EndAbility();
}

void UGA_GroundBlast::TargetCancelled(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	UE_LOG(LogTemp, Warning, TEXT("Target cancelled"));
	K2_EndAbility();
}
