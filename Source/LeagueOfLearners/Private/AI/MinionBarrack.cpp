// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/MinionBarrack.h"
#include "AI/Minion.h"
#include "GameFramework/PlayerStart.h"

// Sets default values
AMinionBarrack::AMinionBarrack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMinionBarrack::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) {
		GetWorldTimerManager().SetTimer(SpawnTimerHandle,this, &AMinionBarrack::SpawnNewGroup,GroupSpawnInterval,true);
	}
}

// Called every frame
void AMinionBarrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

const APlayerStart* AMinionBarrack::GetNextSpawnSpot()
{
	if (SpawnSpots.Num() == 0) {
		return nullptr;
	}
	++NextSpawnSpotIndex;
	if (NextSpawnSpotIndex >= SpawnSpots.Num()) NextSpawnSpotIndex = 0;
	return SpawnSpots[NextSpawnSpotIndex];
}

void AMinionBarrack::SpawnMinions(int Amount)
{
	for (int i = 0; i < Amount; i++) {
		FTransform SpawnTransform = GetActorTransform();
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot()) {
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}
		AMinion* NewMinion = GetWorld()->SpawnActorDeferred<AMinion>(MinionClass,SpawnTransform,this,nullptr,ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		NewMinion->SetGenericTeamId(BarrackTeamID);
		NewMinion->FinishSpawning(SpawnTransform);
		MinionPool.Add(NewMinion);
	}
}

void AMinionBarrack::SpawnNewGroup()
{
	int i = SpawnAmount;
	while (i > 0) {
		FTransform SpawnTransform = GetActorTransform();
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot()) {
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}
		AMinion* NextAvailableMinion = GetNextAvailableMinion();
		if (!NextAvailableMinion) break;
		NextAvailableMinion->SetActorTransform(SpawnTransform);
		NextAvailableMinion->Activate();
		--i;
	}
	SpawnMinions(i);
}

AMinion* AMinionBarrack::GetNextAvailableMinion() const
{
	for (AMinion* Minion : MinionPool) {
		if (!Minion->isActive()) {
			return Minion;
		}
	}
	return nullptr;
}

