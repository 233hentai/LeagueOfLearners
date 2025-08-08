// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/MMC_LevelBased.h"
#include "GAS/LOLHeroAttributeSet.h"
#include "AbilitySystemComponent.h"

UMMC_LevelBased::UMMC_LevelBased()
{
	LevelCaptureDefinition.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	LevelCaptureDefinition.AttributeToCapture = ULOLHeroAttributeSet::GetLevelAttribute();
	RelevantAttributesToCapture.Add(LevelCaptureDefinition);

}

float UMMC_LevelBased::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	UAbilitySystemComponent* ASC = Spec.GetContext().GetInstigatorAbilitySystemComponent();
	if (!ASC) return 0.f;
	float Level = 0;
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	GetCapturedAttributeMagnitude(LevelCaptureDefinition, Spec, EvalParams, Level);

	bool bFound;
	float RateAttributeValue = ASC->GetGameplayAttributeValue(RateAttribute, bFound);
	if (!bFound) return 0.f;
	return (Level - 1) * RateAttributeValue;
}


