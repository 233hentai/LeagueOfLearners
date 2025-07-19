// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/OverHeadStatsGauge.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Widgets/ValueGauge.h"
#include "GAS/LOLAttributeSet.h"

void UOverHeadStatsGauge::ConfigureWithASC(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (AbilitySystemComponent) {
		HealthBar->SetAndBoundToGameplayAttribute(AbilitySystemComponent, ULOLAttributeSet::GetHealthAttribute(), ULOLAttributeSet::GetMaxHealthAttribute());
		ManaBar->SetAndBoundToGameplayAttribute(AbilitySystemComponent, ULOLAttributeSet::GetManaAttribute(), ULOLAttributeSet::GetMaxManaAttribute());
	}
}
