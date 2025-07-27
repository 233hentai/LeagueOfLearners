// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Minion.h"
#include "AIController.h"
#include "BehaviorTree/BlackBoardComponent.h"

void AMinion::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	Super::SetGenericTeamId(NewTeamID);
	PickSkinBasedOnTeamID();
}

bool AMinion::isActive() const
{
	return !IsDead();
}

void AMinion::Activate() {
	RespawnImmediately();
}

void AMinion::SetGoal(AActor* Goal)
{
	if (AAIController* AIController = GetController<AAIController>()) {
		if (UBlackboardComponent* BlckBoard = AIController->GetBlackboardComponent()) {
			BlckBoard->SetValueAsObject(GoalBlackboardKeyName,Goal);
		}
	}
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
