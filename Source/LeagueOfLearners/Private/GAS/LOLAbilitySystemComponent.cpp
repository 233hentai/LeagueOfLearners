// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/LOLAbilitySystemComponent.h"
#include "GAS/LOLAttributeSet.h"

ULOLAbilitySystemComponent::ULOLAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(ULOLAttributeSet::GetHealthAttribute()).AddUObject(this,&ULOLAbilitySystemComponent::HealthUpdated);
}

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

void ULOLAbilitySystemComponent::ApplyFullStatEffect()
{
	AuthApplyGameplayEffect(FullStatEffect);
}

void ULOLAbilitySystemComponent::AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> Effect, int Level)
{
	if (GetOwner() && GetOwner()->HasAuthority()) {
		if (Effect) {
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(Effect, Level, MakeEffectContext());
			ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		}
	}
}

void ULOLAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner()) return;
	if (GetOwner()->HasAuthority() && ChangeData.NewValue <= 0 && DeathEffect) {
		AuthApplyGameplayEffect(DeathEffect);
	}
}
