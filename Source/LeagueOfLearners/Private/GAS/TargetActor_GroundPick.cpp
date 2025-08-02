// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TargetActor_GroundPick.h"
#include "Engine/OverlapResult.h"
#include "LeagueOfLearners/LeagueOfLearners.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"

ATargetActor_GroundPick::ATargetActor_GroundPick()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ATargetActor_GroundPick::SetTargetAreaRadius(float NewRadius)
{
	TargetAreaRadius = NewRadius;
}

void ATargetActor_GroundPick::ConfirmTargetingAndContinue()
{
	TArray<FOverlapResult> OverlapResults;
	FCollisionObjectQueryParams QueryParams;
	QueryParams.AddObjectTypesToQuery(ECC_Pawn);
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(TargetAreaRadius);
	GetWorld()->OverlapMultiByObjectType(OverlapResults,GetActorLocation(),FQuat::Identity, QueryParams, CollisionShape);

	TSet<AActor*> TargetActors;
	IGenericTeamAgentInterface* OwnerTeamInterface = nullptr; 
	if (OwningAbility) {
		OwnerTeamInterface=Cast<IGenericTeamAgentInterface>(OwningAbility->GetAvatarActorFromActorInfo());
	}
	for (const FOverlapResult& OverlapResult : OverlapResults) {
		if(OwnerTeamInterface&&OwnerTeamInterface->GetTeamAttitudeTowards(*OverlapResult.GetActor())==ETeamAttitude::Friendly&&!bShouldTargetFriend) continue;
		if (OwnerTeamInterface && OwnerTeamInterface->GetTeamAttitudeTowards(*OverlapResult.GetActor()) == ETeamAttitude::Hostile && !bShouldTargetEnemy) continue;
		TargetActors.Add(OverlapResult.GetActor());
	}
	FGameplayAbilityTargetDataHandle TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(TargetActors.Array(),false);
	TargetDataReadyDelegate.Broadcast(TargetData);
}

void ATargetActor_GroundPick::SetTargetOptions(bool bTargetFriend, bool bTargetEnemy)
{
	bShouldTargetFriend = bTargetFriend;
	bShouldTargetEnemy = bTargetEnemy;
}

void ATargetActor_GroundPick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PrimaryPC && PrimaryPC->IsLocalPlayerController()) {
		SetActorLocation(GetTargetPoint());
	}
}

FVector ATargetActor_GroundPick::GetTargetPoint() const
{
	if (!PrimaryPC || !PrimaryPC->IsLocalPlayerController()) return GetActorLocation();

	FHitResult TraceResult;
	FVector ViewLocation;
	FRotator ViewRotation;
	PrimaryPC->GetPlayerViewPoint(ViewLocation,ViewRotation);
	FVector TraceEnd= ViewLocation+ViewRotation.Vector()*TargetTraceRange;
	GetWorld()->LineTraceSingleByChannel(TraceResult, ViewLocation, TraceEnd, ECC_Target);

	if (!TraceResult.bBlockingHit) {
		GetWorld()->LineTraceSingleByChannel(TraceResult, TraceEnd, TraceEnd+FVector::DownVector*TNumericLimits<float>::Max(), ECC_Target);
	}
	if (!TraceResult.bBlockingHit) {
		return GetActorLocation();
	}

	return TraceResult.ImpactPoint;
}
