// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Uppercut.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

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
		}
	}
}
