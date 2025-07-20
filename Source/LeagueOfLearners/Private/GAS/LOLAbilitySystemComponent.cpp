// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/LOLAbilitySystemComponent.h"

void ULOLAbilitySystemComponent::ApplyInitialEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) {
		return;
	}
	for (const TSubclassOf<UGameplayEffect>& EffectClass: InitialEffects) {
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectClass,1,MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void ULOLAbilitySystemComponent::GiveInitialAbilities()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) {
		return;
	}
	for (const TPair<ELOLAbilityInputID,TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities) {
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value,0,(int32)AbilityPair.Key, nullptr));
	}
	for (const TPair<ELOLAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : BasicAbilities) {
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 1, (int32)AbilityPair.Key, nullptr));
	}
}
