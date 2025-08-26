// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LOLAbilitySystemStatics.generated.h"

class UGameplayAbility;
struct FGameplayAbilitySpec;
class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class ULOLAbilitySystemStatics:public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static FGameplayTag GetBasicAttackAbilityTag();
	static FGameplayTag GetDeadStatTag();
	static FGameplayTag GetStunStatTag();
	static FGameplayTag GetBasicAttackInputPressedTag();
	static FGameplayTag GetBasicAttackInputReleasedTag();
	static FGameplayTag GetAimStatTag();
	static FGameplayTag GetFocusStatTag();
	static FGameplayTag GetCameraShakeCueTag();
	static FGameplayTag GetHealthFullTag();
	static FGameplayTag GetHealthEmptyTag();
	static FGameplayTag GetManaFullTag();
	static FGameplayTag GetManaEmptyTag();
	static FGameplayTag GetHeroRoleTag();
	static FGameplayTag GetExperienceAttributeTag();
	static FGameplayTag GetGoldAttributeTag();
	static FGameplayTag GetCrosshairTag();
	static FGameplayTag GetTargetUpdatedTag();

	static bool IsActorDead(AActor* Actor);
	static bool IsHero(const AActor* Actor);
	static bool ActorHasTag(const AActor* ActorToCheck, const FGameplayTag& Tag);
	static bool IsAbilityAtMaxLevel(const FGameplayAbilitySpec& Spec);

	static float GetStaticCooldownDurationForAbility(const class UGameplayAbility* Ability);
	static float GetStaticCostForAbility(const class UGameplayAbility* Ability);

	static bool CheckCost(const FGameplayAbilitySpec& AbilitySpec,const UAbilitySystemComponent& ASC);
	static bool CheckCostStatic(const UGameplayAbility* AbilityCDO,const UAbilitySystemComponent& ASC);
	static float GetManaCostFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel);
	static float GetCooldownDurationFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC, int AbilityLevel);
	static float GetCooldownRemaining(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC);
};
