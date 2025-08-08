// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GAP_Dead.h"
#include "GAS/LOLAbilitySystemStatics.h"
#include "Engine/OverlapResult.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/LOLAttributeSet.h"
#include "GAS/LOLHeroAttributeSet.h"

UGAP_Dead::UGAP_Dead()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = ULOLAbilitySystemStatics::GetDeadStatTag();
	AbilityTriggers.Add(TriggerData);
	ActivationBlockedTags.RemoveTag(ULOLAbilitySystemStatics::GetStunStatTag());
}

void UGAP_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (K2_HasAuthority()) {
		AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser();
		if (!Killer||!ULOLAbilitySystemStatics::IsHero(Killer)) {
			Killer = nullptr;
		}
		TArray<AActor*> RewardTargets = GetRewardTargets();
		if (RewardTargets.Num() == 0 && !Killer) {
			K2_EndAbility();
			return;
		}
		if (Killer && !RewardTargets.Contains(Killer)) {
			RewardTargets.Add(Killer);
		}bool bFound;
		float SelfExperience = GetAbilitySystemComponentFromActorInfo_Ensured()->GetGameplayAttributeValue(ULOLHeroAttributeSet::GetExperienceAttribute(), bFound);
		float TotalExperienceReward = BaseExperienceReward + ExperienceRewardPerExperience * SelfExperience;
		float TotalGoldReward = BaseGoldReward + GoldRewardPerExperience * SelfExperience;
		if (Killer) {
			float KillerGoldReward = TotalGoldReward * KillerRewardPortion;
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffect);
			EffectSpecHandle.Data->SetSetByCallerMagnitude(ULOLAbilitySystemStatics::GetGoldAttributeTag(),KillerGoldReward);
			EffectSpecHandle.Data->SetSetByCallerMagnitude(ULOLAbilitySystemStatics::GetExperienceAttributeTag(), 0);
			K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle,UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));
			TotalGoldReward -= KillerGoldReward;
		}
		float ExperiencePerTarget = TotalExperienceReward / RewardTargets.Num();
		float GoldPerTarget = TotalGoldReward / RewardTargets.Num();
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(RewardEffect);
		EffectSpecHandle.Data->SetSetByCallerMagnitude(ULOLAbilitySystemStatics::GetExperienceAttributeTag(), ExperiencePerTarget);
		EffectSpecHandle.Data->SetSetByCallerMagnitude(ULOLAbilitySystemStatics::GetGoldAttributeTag(), GoldPerTarget);
		K2_ApplyGameplayEffectSpecToTarget(EffectSpecHandle,UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(RewardTargets,true));
		K2_EndAbility();
	}
}

TArray<AActor*> UGAP_Dead::GetRewardTargets() const
{
	TSet<AActor*> OutActors;
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if(!AvatarActor||!GetWorld()){
		return OutActors.Array();
	}
	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(RewardRange);

	TArray<FOverlapResult> OverlapResults;
	if (GetWorld()->OverlapMultiByObjectType(OverlapResults, AvatarActor->GetActorLocation(), FQuat::Identity, QueryParams, CollisionShape)) {
		for (const FOverlapResult& OverlapResult : OverlapResults) {
			IGenericTeamAgentInterface* TeamInterface = Cast<IGenericTeamAgentInterface>(OverlapResult.GetActor());
			if (!TeamInterface || TeamInterface->GetTeamAttitudeTowards(*AvatarActor) != ETeamAttitude::Hostile) continue;
			if (!ULOLAbilitySystemStatics::IsHero(OverlapResult.GetActor())) continue;
			OutActors.Add(OverlapResult.GetActor());
		}
	}
	return OutActors.Array();
}
