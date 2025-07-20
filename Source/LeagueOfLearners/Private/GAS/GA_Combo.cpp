// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_Combo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "GameplayTagsManager.h"
#include "GAS/LOLAbilitySystemStatics.h"

UGA_Combo::UGA_Combo()
{
	AbilityTags.AddTag(ULOLAbilitySystemStatics::GetBasicAttackAbilityTag());
	BlockAbilitiesWithTag.AddTag(ULOLAbilitySystemStatics::GetBasicAttackAbilityTag());
}

void UGA_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility()) {
		K2_EndAbility();
		return;
	}
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo)) {
		UAbilityTask_PlayMontageAndWait* PlayCombo_PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMotage);
		PlayCombo_PlayMontageAndWait->OnBlendOut.AddDynamic(this,&UGA_Combo::K2_EndAbility);
		PlayCombo_PlayMontageAndWait->OnCancelled.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayCombo_PlayMontageAndWait->OnCompleted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayCombo_PlayMontageAndWait->OnInterrupted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayCombo_PlayMontageAndWait->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitComboChangeEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,GetComboChangedEventTag(),nullptr,false,false);
		WaitComboChangeEventTask->EventReceived.AddDynamic(this,&UGA_Combo::ComboChangedEventReceived);
		WaitComboChangeEventTask->ReadyForActivation();
	}
	SetupWaitComboInputPress();
}

FGameplayTag UGA_Combo::GetComboChangedEventTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.Combo.Change");
}

FGameplayTag UGA_Combo::GetComboChangedEventEndTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.Combo.Change.End");
}

void UGA_Combo::SetupWaitComboInputPress()
{
	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this,&UGA_Combo::HandleInputPress);
	WaitInputPress->ReadyForActivation();
}

void UGA_Combo::HandleInputPress(float TimeWaited)
{
	SetupWaitComboInputPress();
	TryCommitCombo();
}

void UGA_Combo::TryCommitCombo()
{
	if (NextComboName == NAME_None) {
		return;
	}
	UAnimInstance* OwnerAnimInstace = GetOwnerAnimInstance();
	if (!OwnerAnimInstace) return;
	OwnerAnimInstace->Montage_SetNextSection(OwnerAnimInstace->Montage_GetCurrentSection(ComboMotage),NextComboName,ComboMotage);
}

void UGA_Combo::ComboChangedEventReceived(FGameplayEventData Data)
{
	FGameplayTag EventTag = Data.EventTag;
	if (EventTag == GetComboChangedEventEndTag()) {
		NextComboName = NAME_None;
		UE_LOG(LogTemp, Warning, TEXT("Next combo is cleared"));
		return;
	}
	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
	NextComboName = TagNames.Last();
	UE_LOG(LogTemp, Warning, TEXT("Next combo is %s"),*NextComboName.ToString());
}
