// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/LOLAbilitySystemComponent.h"
#include "GAS/LOLAttributeSet.h"
#include "GAS/LOLGameplayAbilityTypes.h"
#include "GAS/LOLHeroAttributeSet.h"
#include "GAS/LOLAbilitySystemStatics.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/PA_AbilitySystemGenerics.h"

ULOLAbilitySystemComponent::ULOLAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(ULOLAttributeSet::GetHealthAttribute()).AddUObject(this,&ULOLAbilitySystemComponent::HealthUpdated);
	GetGameplayAttributeValueChangeDelegate(ULOLAttributeSet::GetManaAttribute()).AddUObject(this,&ULOLAbilitySystemComponent::ManaUpdated);
	GetGameplayAttributeValueChangeDelegate(ULOLHeroAttributeSet::GetExperienceAttribute()).AddUObject(this,&ULOLAbilitySystemComponent::ExperienceUpdated);
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
	if (!AbilitySystemGenerics|| !AbilitySystemGenerics->GetBaseStatsDataTable()||!GetOwner()) return;
	const UDataTable* BaseStatsDataTable = AbilitySystemGenerics->GetBaseStatsDataTable();
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
	const FRealCurve* ExperienceCurve = AbilitySystemGenerics->GetExperienceCurve();
	if (ExperienceCurve) {
		int MaxLevel = ExperienceCurve->GetNumKeys();
		SetNumericAttributeBase(ULOLHeroAttributeSet::GetMaxLevelAttribute(), MaxLevel);
		float MaxLevelExperience = ExperienceCurve->GetKeyValue(ExperienceCurve->GetLastKeyHandle());
		SetNumericAttributeBase(ULOLHeroAttributeSet::GetMaxLevelExperienceAttribute(), MaxLevelExperience);
		//UE_LOG(LogTemp, Warning, TEXT("max level is %d, max level exp is %f"),MaxLevel,MaxLevelExperience);
	}
	ExperienceUpdated(FOnAttributeChangeData());
}

void ULOLAbilitySystemComponent::ApplyInitialEffects()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) {
		return;
	}
	if (!AbilitySystemGenerics) return;
	for (const TSubclassOf<UGameplayEffect>& EffectClass: AbilitySystemGenerics->GetInitialEffects()) {
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
	if (!AbilitySystemGenerics) return;
	for (const TSubclassOf<UGameplayAbility>& PassiveAbility : AbilitySystemGenerics->GetPassiveAbilities()) {
		GiveAbility(FGameplayAbilitySpec(PassiveAbility,1,-1,nullptr));
	}
}

void ULOLAbilitySystemComponent::ApplyFullStatEffect()
{
	if (!AbilitySystemGenerics) return;
	AuthApplyGameplayEffect(AbilitySystemGenerics->GetFullStatEffect());
}


const TMap<ELOLAbilityInputID, TSubclassOf<UGameplayAbility>>& ULOLAbilitySystemComponent::GetAbilities() const
{
	return Abilities;
}

bool ULOLAbilitySystemComponent::IsAtMaxLevel() const
{
	bool bFound;
	float CurrentLevel = GetGameplayAttributeValue(ULOLHeroAttributeSet::GetLevelAttribute(),bFound);
	float MaxLevel = GetGameplayAttributeValue(ULOLHeroAttributeSet::GetMaxLevelAttribute(),bFound);
	return CurrentLevel >= MaxLevel;
}

void ULOLAbilitySystemComponent::Server_UpGradeAbilityWithInputID_Implementation(ELOLAbilityInputID InputID)
{
	bool bFound = false;
	float UpgradePoint = GetGameplayAttributeValue(ULOLHeroAttributeSet::GetUpgradePointAttribute(),bFound);
	if (!bFound||UpgradePoint<=0) return;
	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromInputID((int32)InputID);
	if (!AbilitySpec||ULOLAbilitySystemStatics::IsAbilityAtMaxLevel(*AbilitySpec)) return;
	SetNumericAttributeBase(ULOLHeroAttributeSet::GetUpgradePointAttribute(), UpgradePoint - 1);
	//UE_LOG(LogTemp, Warning, TEXT("Upgrade Point now is: %f"),GetGameplayAttributeValue(ULOLHeroAttributeSet::GetUpgradePointAttribute(),bFound));
	AbilitySpec->Level += 1;
	MarkAbilitySpecDirty(*AbilitySpec);
	Client_AbilitySpecLevelUpdated(AbilitySpec->Handle,AbilitySpec->Level);
}

bool ULOLAbilitySystemComponent::Server_UpGradeAbilityWithInputID_Validate(ELOLAbilityInputID InputID)
{
	return true;
}

void ULOLAbilitySystemComponent::Client_AbilitySpecLevelUpdated_Implementation(FGameplayAbilitySpecHandle SpecHandle, int NewLevel)
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle);
	if (Spec) {
		Spec->Level = NewLevel;
		AbilitySpecDirtiedCallbacks.Broadcast(*Spec);
	}
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
			if (AbilitySystemGenerics&&AbilitySystemGenerics->GetDeathEffect()) {
				AuthApplyGameplayEffect(AbilitySystemGenerics->GetDeathEffect());
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

void ULOLAbilitySystemComponent::ExperienceUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (IsAtMaxLevel()) return;
	if (!AbilitySystemGenerics) return;
	float CurrentExp = ChangeData.NewValue;
	//UE_LOG(LogTemp, Warning, TEXT("CurrentExp:%f"),CurrentExp);
	const FRealCurve* ExperienceCurve = AbilitySystemGenerics->GetExperienceCurve();
	if (!ExperienceCurve) {
		UE_LOG(LogTemp, Warning, TEXT("Can't find EXP Data!"));
		return;
	}
	float PrevLevelEXP = 0;
	float NextLevelEXP = 0;
	float NewLevel = 1;
	for (auto Iter = ExperienceCurve->GetKeyHandleIterator(); Iter; ++Iter) {
		float EXPToReachLevel = ExperienceCurve->GetKeyValue(*Iter);
		if (CurrentExp < EXPToReachLevel) {
			NextLevelEXP = EXPToReachLevel;
			break;
		}
		PrevLevelEXP = EXPToReachLevel;
		NewLevel = Iter.GetIndex() + 1;
	}
	float CurrentLevel = GetNumericAttributeBase(ULOLHeroAttributeSet::GetLevelAttribute());
	float CurrentUpgradePoint = GetNumericAttributeBase(ULOLHeroAttributeSet::GetUpgradePointAttribute());
	float LevelUpgraded = NewLevel - CurrentLevel;
	float NewUpgradePoint = CurrentUpgradePoint + LevelUpgraded;
	//UE_LOG(LogTemp, Warning, TEXT("NewUpgradePoint:%f, CurrentUpgradePoint:%f, LevelUpgraded:%f"), NewUpgradePoint, CurrentUpgradePoint, LevelUpgraded);
	SetNumericAttributeBase(ULOLHeroAttributeSet::GetLevelAttribute(),NewLevel);
	SetNumericAttributeBase(ULOLHeroAttributeSet::GetPrevLevelExperienceAttribute(), PrevLevelEXP);
	//UE_LOG(LogTemp, Warning, TEXT("NextLevelEXP:%f"), NextLevelEXP);
	SetNumericAttributeBase(ULOLHeroAttributeSet::GetNextLevelExperienceAttribute(), NextLevelEXP);
	SetNumericAttributeBase(ULOLHeroAttributeSet::GetUpgradePointAttribute(), NewUpgradePoint);
	//UE_LOG(LogTemp, Warning, TEXT("Upgrade Point now is %f"), GetNumericAttributeBase(ULOLHeroAttributeSet::GetUpgradePointAttribute()));
}
