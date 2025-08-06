// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/LOLAbilitySystemComponent.h"
#include "GAS/LOLAttributeSet.h"
#include "GAS/LOLGameplayAbilityTypes.h"
#include "GAS/LOLHeroAttributeSet.h"
#include "GAS/LOLAbilitySystemStatics.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"

ULOLAbilitySystemComponent::ULOLAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(ULOLAttributeSet::GetHealthAttribute()).AddUObject(this,&ULOLAbilitySystemComponent::HealthUpdated);
	GetGameplayAttributeValueChangeDelegate(ULOLAttributeSet::GetManaAttribute()).AddUObject(this,&ULOLAbilitySystemComponent::ManaUpdated);
	GenericConfirmInputID = (int32)ELOLAbilityInputID::Confirm;
	GenericCancelInputID = (int32)ELOLAbilityInputID::Cancel;
}

void ULOLAbilitySystemComponent::ServerInit()
{
	InitializeBaseAttributes();
	ApplyInitialEffects();
	GiveInitialAbilities();
}

void ULOLAbilitySystemComponent::InitializeBaseAttributes()
{
	if (!BaseStatsDataTable||!GetOwner()) return;
	const FHeroBaseStats* BaseStats = nullptr;
	for (const TPair<FName, uint8*>& DataPair : BaseStatsDataTable->GetRowMap()) {
		BaseStats = BaseStatsDataTable->FindRow<FHeroBaseStats>(DataPair.Key,"");
		if (BaseStats && BaseStats->Class == GetOwner()->GetClass()) break;
	}
	if (BaseStats) {
		SetNumericAttributeBase(ULOLAttributeSet::GetMaxHealthAttribute(),BaseStats->BaseMaxHealth);
		SetNumericAttributeBase(ULOLAttributeSet::GetMaxManaAttribute(), BaseStats->BaseMaxMana);
		SetNumericAttributeBase(ULOLAttributeSet::GetAttackAttribute(), BaseStats->BaseAttack);
		SetNumericAttributeBase(ULOLAttributeSet::GetArmorAttribute(), BaseStats->BaseArmor);
		SetNumericAttributeBase(ULOLAttributeSet::GetMoveSpeedAttribute(), BaseStats->MoveSpeed);
		SetNumericAttributeBase(ULOLHeroAttributeSet::GetStrengthAttribute(), BaseStats->Strength);
		SetNumericAttributeBase(ULOLHeroAttributeSet::GetStrengthGrowthRateAttribute(), BaseStats->StrengthGrowthRate);
		SetNumericAttributeBase(ULOLHeroAttributeSet::GetIntelligenceAttribute(), BaseStats->Intelligence);
		SetNumericAttributeBase(ULOLHeroAttributeSet::GetIntelligenceGrowthRateAttribute(), BaseStats->IntelligenceGrowthRate);
	}
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
	for (const TSubclassOf<UGameplayAbility>& PassiveAbility : PassiveAbilities) {
		GiveAbility(FGameplayAbilitySpec(PassiveAbility,1,-1,nullptr));
	}
}

void ULOLAbilitySystemComponent::ApplyFullStatEffect()
{
	AuthApplyGameplayEffect(FullStatEffect);
}


const TMap<ELOLAbilityInputID, TSubclassOf<UGameplayAbility>>& ULOLAbilitySystemComponent::GetAbilities() const
{
	return Abilities;
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
	if (!GetOwner()||!GetOwner()->HasAuthority()) return;
	bool bFound = false;
	float MaxHealth = GetGameplayAttributeValue(ULOLAttributeSet::GetMaxHealthAttribute(),bFound);
	if (bFound && ChangeData.NewValue >= MaxHealth) {
		if (!HasMatchingGameplayTag(ULOLAbilitySystemStatics::GetHealthFullTag())) {
			AddLooseGameplayTag(ULOLAbilitySystemStatics::GetHealthFullTag());
		}
	}
	else {
		RemoveLooseGameplayTag(ULOLAbilitySystemStatics::GetHealthFullTag());
	}
	if (ChangeData.NewValue <= 0) {
		if (!HasMatchingGameplayTag(ULOLAbilitySystemStatics::GetHealthEmptyTag())) {
			AddLooseGameplayTag(ULOLAbilitySystemStatics::GetHealthEmptyTag());
			if (DeathEffect) {
				AuthApplyGameplayEffect(DeathEffect);
			}
			FGameplayEventData DeadAbilityEventData;
			if (ChangeData.GEModData) {
				DeadAbilityEventData.ContextHandle = ChangeData.GEModData->EffectSpec.GetContext();
			}
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(),ULOLAbilitySystemStatics::GetDeadStatTag(), DeadAbilityEventData);
		}
	}
	else {
		RemoveLooseGameplayTag(ULOLAbilitySystemStatics::GetHealthEmptyTag());
	}
}

void ULOLAbilitySystemComponent::ManaUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	bool bFound = false;
	float MaxMana = GetGameplayAttributeValue(ULOLAttributeSet::GetMaxManaAttribute(), bFound);
	if (bFound && ChangeData.NewValue >= MaxMana) {
		if (!HasMatchingGameplayTag(ULOLAbilitySystemStatics::GetManaFullTag())) {
			AddLooseGameplayTag(ULOLAbilitySystemStatics::GetManaFullTag());
		}
	}
	else {
		RemoveLooseGameplayTag(ULOLAbilitySystemStatics::GetManaFullTag());
	}
	if (ChangeData.NewValue <= 0) {
		if (!HasMatchingGameplayTag(ULOLAbilitySystemStatics::GetManaEmptyTag())) {
			AddLooseGameplayTag(ULOLAbilitySystemStatics::GetManaEmptyTag());
		}
	}
	else {
		RemoveLooseGameplayTag(ULOLAbilitySystemStatics::GetManaEmptyTag());
	}
}
