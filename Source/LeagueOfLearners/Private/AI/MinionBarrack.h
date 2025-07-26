// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "MinionBarrack.generated.h"

UCLASS()
class AMinionBarrack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMinionBarrack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	UPROPERTY(EditAnywhere, Category = "Spawn")
	FGenericTeamId BarrackTeamID;
	UPROPERTY()
	TArray<class AMinion*> MinionPool;
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<class AMinion> MinionClass;
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TArray<APlayerStart*> SpawnSpots;
	UPROPERTY(EditAnywhere, Category = "Spawn")
	int SpawnAmount = 5;
	UPROPERTY(EditAnywhere, Category = "Spawn")
	float GroupSpawnInterval=10.f;

	int NextSpawnSpotIndex = -1;
	const APlayerStart* GetNextSpawnSpot();
	void SpawnMinions(int Amount);
	void SpawnNewGroup();
	FTimerHandle SpawnTimerHandle;
	AMinion* GetNextAvailableMinion() const; 

};
