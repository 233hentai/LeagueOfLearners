// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LevelGauge.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/LOLHeroAttributeSet.h"

void ULevelGauge::NativeConstruct()
{
	Super::NativeConstruct();
	NumberFormattingOptions.SetMaximumFractionalDigits(0);
	APawn* OwnerPawn = GetOwningPlayerPawn();
	if (!OwnerPawn) return;
	UAbilitySystemComponent* OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPawn);
	if (!OwnerAbilitySystemComponent) return;
	OwnerASC = OwnerAbilitySystemComponent;
	UpdateGauge(FOnAttributeChangeData());
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(ULOLHeroAttributeSet::GetExperienceAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge);
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(ULOLHeroAttributeSet::GetPrevLevelExperienceAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge);
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(ULOLHeroAttributeSet::GetNextLevelExperienceAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge);
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(ULOLHeroAttributeSet::GetLevelAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge);
}

void ULevelGauge::UpdateGauge(const FOnAttributeChangeData& Data)
{
	bool bFound;
	float CurrentEXP = OwnerASC->GetGameplayAttributeValue(ULOLHeroAttributeSet::GetExperienceAttribute(),bFound);
	if (!bFound) return;
	float NextLevelEXP = OwnerASC->GetGameplayAttributeValue(ULOLHeroAttributeSet::GetNextLevelExperienceAttribute(), bFound);
	if (!bFound) return;
	float PrevLevelEXP = OwnerASC->GetGameplayAttributeValue(ULOLHeroAttributeSet::GetPrevLevelExperienceAttribute(), bFound);
	if (!bFound) return;
	float CurrentLevel = OwnerASC->GetGameplayAttributeValue(ULOLHeroAttributeSet::GetLevelAttribute(), bFound);
	if (!bFound) return;

	LevelText->SetText(FText::AsNumber(CurrentLevel, &NumberFormattingOptions));
	float Progress = CurrentEXP - PrevLevelEXP;
	float LevelEXPAmount = NextLevelEXP - PrevLevelEXP;
	float Percent = Progress / LevelEXPAmount;
	if (NextLevelEXP == 0) {
		Percent = 1;
	}
	if (LevelProgressImage) {
		LevelProgressImage->GetDynamicMaterial()->SetScalarParameterValue(PercentMaterialParamName, Percent);
	}
}
