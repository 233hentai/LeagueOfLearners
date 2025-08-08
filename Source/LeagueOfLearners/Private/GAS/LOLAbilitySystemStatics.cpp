// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/LOLAbilitySystemStatics.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"


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

FGameplayTag ULOLAbilitySystemStatics::GetAimStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Aim");
}

FGameplayTag ULOLAbilitySystemStatics::GetCameraShakeCueTag()
{
	return FGameplayTag::RequestGameplayTag("GameplayCue.CameraShake");
}

FGameplayTag ULOLAbilitySystemStatics::GetHealthFullTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Health.Full");
}

FGameplayTag ULOLAbilitySystemStatics::GetHealthEmptyTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Health.Empty");
}

FGameplayTag ULOLAbilitySystemStatics::GetManaFullTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Mana.Full");
}

FGameplayTag ULOLAbilitySystemStatics::GetManaEmptyTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Mana.Empty");
}

FGameplayTag ULOLAbilitySystemStatics::GetHeroRoleTag()
{
	return FGameplayTag::RequestGameplayTag("Role.Hero");
}

FGameplayTag ULOLAbilitySystemStatics::GetExperienceAttributeTag()
{
	return FGameplayTag::RequestGameplayTag("Attribute.Experience");
}

FGameplayTag ULOLAbilitySystemStatics::GetGoldAttributeTag()
{
	return FGameplayTag::RequestGameplayTag("Attribute.Gold");
}

bool ULOLAbilitySystemStatics::IsHero(const AActor* Actor)
{
	const IAbilitySystemInterface* ActorASI = Cast<IAbilitySystemInterface>(Actor);
	if (ActorASI) {
		UAbilitySystemComponent* ActorASC = ActorASI->GetAbilitySystemComponent();
		if (ActorASC) {
			return ActorASC->HasMatchingGameplayTag(GetHeroRoleTag());
		}
	}
	return false;
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


