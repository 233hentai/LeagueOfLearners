// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Minion.h"
#include "AbilitySystemComponent.h"
#include "GAS/LOLAbilitySystemStatics.h"

void AMinion::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	Super::SetGenericTeamId(NewTeamID);
	PickSkinBasedOnTeamID();
}

bool AMinion::isActive() const
{
	return !GetAbilitySystemComponent()->HasMatchingGameplayTag(ULOLAbilitySystemStatics::GetDeadStatTag());
}

void AMinion::Activate() {
	GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(ULOLAbilitySystemStatics::GetDeadStatTag()));
}

void AMinion::PickSkinBasedOnTeamID()
{
	USkeletalMesh** Skin = SkinMap.Find(GetGenericTeamId());
	if (Skin) {
		GetMesh()->SetSkeletalMesh(*Skin);
	}
}

void AMinion::OnRep_TeamID()
{
	PickSkinBasedOnTeamID();
}
