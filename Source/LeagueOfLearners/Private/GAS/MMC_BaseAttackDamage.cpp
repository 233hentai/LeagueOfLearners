// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/MMC_BaseAttackDamage.h"
#include "GAS/LOLAttributeSet.h"

UMMC_BaseAttackDamage::UMMC_BaseAttackDamage()
{
	AttackCaptureDef.AttributeToCapture = ULOLAttributeSet::GetAttackAttribute();
	AttackCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	ArmorCaptureDef.AttributeToCapture = ULOLAttributeSet::GetArmorAttribute();
	ArmorCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	RelevantAttributesToCapture.Add(AttackCaptureDef);
	RelevantAttributesToCapture.Add(ArmorCaptureDef);

}

float UMMC_BaseAttackDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	float Attack = 0.f;
	float Armor = 0.f;
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	GetCapturedAttributeMagnitude(AttackCaptureDef,Spec,EvaluateParameters,Attack);
	GetCapturedAttributeMagnitude(ArmorCaptureDef,Spec,EvaluateParameters,Armor);
	float Damage = Attack * (1 - Armor / (Armor + 100));
	return -Damage;
}
