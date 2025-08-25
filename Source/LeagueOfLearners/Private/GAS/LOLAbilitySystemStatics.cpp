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

FGameplayTag ULOLAbilitySystemStatics::GetBasicAttackInputReleasedTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.BasicAttack.Released");
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

FGameplayTag ULOLAbilitySystemStatics::GetCrosshairTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Crosshair");
}

FGameplayTag ULOLAbilitySystemStatics::GetTargetUpdatedTag()
{
	return FGameplayTag::RequestGameplayTag("Target.Updated");
}

bool ULOLAbilitySystemStatics::IsActorDead(AActor* Actor)
{
	return ActorHasTag(Actor, GetDeadStatTag());
}

bool ULOLAbilitySystemStatics::IsHero(const AActor* Actor)
{
	return ActorHasTag(Actor, GetHeroRoleTag());
}

bool ULOLAbilitySystemStatics::ActorHasTag(const AActor* ActorToCheck, const FGameplayTag& Tag)
{
	const IAbilitySystemInterface* ActorASI = Cast<IAbilitySystemInterface>(ActorToCheck);
	if (ActorASI) {
		UAbilitySystemComponent* ActorASC = ActorASI->GetAbilitySystemComponent();
		if (ActorASC) {
			return ActorASC->HasMatchingGameplayTag(Tag);
		}
	}
	return false;
}

bool ULOLAbilitySystemStatics::IsAbilityAtMaxLevel(const FGameplayAbilitySpec& Spec)
{
	return Spec.Level >= 4;
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

bool ULOLAbilitySystemStatics::CheckCost(const FGameplayAbilitySpec& AbilitySpec, const UAbilitySystemComponent& ASC)
{
	const UGameplayAbility* AbilityCDO = AbilitySpec.Ability;
	if (AbilityCDO) {
		return AbilityCDO->CheckCost(AbilitySpec.Handle, ASC.AbilityActorInfo.Get());
	}
	return false;
}

bool ULOLAbilitySystemStatics::CheckCostStatic(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC)
{
	if (AbilityCDO) {
		return AbilityCDO->CheckCost(FGameplayAbilitySpecHandle(), ASC.AbilityActorInfo.Get());
	}
	return false;
}

float ULOLAbilitySystemStatics::GetManaCostFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel)
{
	float ManaCost = 0.f;
	if (AbilityCDO) {
		UGameplayEffect* CostEffect = AbilityCDO->GetCostGameplayEffect();
		if (CostEffect) {
			FGameplayEffectSpecHandle EffectSpecHandle = ASC.MakeOutgoingSpec(CostEffect->GetClass(), AbilityLevel, ASC.MakeEffectContext());
			CostEffect->Modifiers[0].ModifierMagnitude.AttemptCalculateMagnitude(*EffectSpecHandle.Data.Get(),ManaCost);
		}
	}
	return FMath::Abs(ManaCost);
}

float ULOLAbilitySystemStatics::GetCooldownDurationFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel)
{
	float CooldownDuration = 0.f;
	if (AbilityCDO) {
		UGameplayEffect* CooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
		if (CooldownEffect) {
			FGameplayEffectSpecHandle EffectSpecHandle = ASC.MakeOutgoingSpec(CooldownEffect->GetClass(), AbilityLevel, ASC.MakeEffectContext());
			CooldownEffect->DurationMagnitude.AttemptCalculateMagnitude(*EffectSpecHandle.Data.Get(), CooldownDuration);
		}
	}
	return FMath::Abs(CooldownDuration);
}

float ULOLAbilitySystemStatics::GetCooldownRemaining(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC)
{
	if (!AbilityCDO) return 0.f;
	UGameplayEffect* CooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
	if (!CooldownEffect) return 0.f;
	FGameplayEffectQuery CooldownEffectQuery;
	CooldownEffectQuery.EffectDefinition = CooldownEffect->GetClass();
	float CooldownRemaining = 0.f;
	FJsonSerializableArrayFloat CooldownRemainings = ASC.GetActiveEffectsTimeRemaining(CooldownEffectQuery);
	for (float Remaining : CooldownRemainings) {
		if (Remaining > CooldownRemaining) {
			CooldownRemaining = Remaining;
		}
	}
	return CooldownRemaining;
}


