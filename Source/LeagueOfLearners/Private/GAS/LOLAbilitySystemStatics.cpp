// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/LOLAbilitySystemStatics.h"
#include "Abilities/GameplayAbility.h"


FGameplayTag ULOLAbilitySystemStatics::GetBasicAttackAbilityTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.BasicAttack");
}

FGameplayTag ULOLAbilitySystemStatics::GetDeadStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Death");
}

FGameplayTag ULOLAbilitySystemStatics::GetStunStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Stun");
}

FGameplayTag ULOLAbilitySystemStatics::GetBasicAttackInputPressedTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.BasicAttack.Pressed");
}

float ULOLAbilitySystemStatics::GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability)
{
	if(!Ability) return 0.0f;
	const UGameplayEffect* CooldownEffect = Ability->GetCooldownGameplayEffect();
	if(!CooldownEffect) return 0.0f;
	float CooldownDuration = 0.0f;
	CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1,CooldownDuration);
	return CooldownDuration;
}

float ULOLAbilitySystemStatics::GetStaticCostForAbility(const UGameplayAbility* Ability)
{
	if (!Ability) return 0.0f;
	const UGameplayEffect* CostEffect = Ability->GetCostGameplayEffect();
	if (!CostEffect|| CostEffect->Modifiers.Num()==0) return 0.0f;
	float Cost = 0.0f;
	CostEffect->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost);//暂时只用第一个modifier
	return FMath::Abs(Cost);
}


