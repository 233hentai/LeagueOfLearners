// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/LOLHeroAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

void ULOLHeroAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ULOLHeroAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULOLHeroAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULOLHeroAttributeSet, Experience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULOLHeroAttributeSet, PrevLevelExperience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULOLHeroAttributeSet, NextLevelExperience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULOLHeroAttributeSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULOLHeroAttributeSet, MaxLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULOLHeroAttributeSet, MaxLevelExperience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULOLHeroAttributeSet, Gold, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULOLHeroAttributeSet, UpgradePoint, COND_None, REPNOTIFY_Always);
}

void ULOLHeroAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULOLHeroAttributeSet, Intelligence, OldValue);
}

void ULOLHeroAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULOLHeroAttributeSet, Strength, OldValue);
}

void ULOLHeroAttributeSet::OnRep_Experience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULOLHeroAttributeSet, Experience, OldValue);
}

void ULOLHeroAttributeSet::OnRep_PrevLevelExperience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULOLHeroAttributeSet, PrevLevelExperience, OldValue);
}

void ULOLHeroAttributeSet::OnRep_NextLevelExperience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULOLHeroAttributeSet, NextLevelExperience, OldValue);
}

void ULOLHeroAttributeSet::OnRep_Level(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULOLHeroAttributeSet, Level, OldValue);
}

void ULOLHeroAttributeSet::OnRep_MaxLevel(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULOLHeroAttributeSet, MaxLevel, OldValue);
}

void ULOLHeroAttributeSet::OnRep_MaxLevelExperience(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULOLHeroAttributeSet, MaxLevelExperience, OldValue);
}

void ULOLHeroAttributeSet::OnRep_Gold(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULOLHeroAttributeSet, Gold, OldValue);
}

void ULOLHeroAttributeSet::OnRep_UpgradePoint(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULOLHeroAttributeSet, Level, OldValue);
}
