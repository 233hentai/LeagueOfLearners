// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/LOLAbilitySystemStatics.h"


FGameplayTag ULOLAbilitySystemStatics::GetBasicAttackAbilityTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.BasicAttack");
}

FGameplayTag ULOLAbilitySystemStatics::GetDeadStatTag()
{
	return FGameplayTag::RequestGameplayTag("Stats.Death");
}
