// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "GameplayEffectTypes.h"
#include "Projectile.generated.h"

UCLASS()
class AProjectile : public AActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	void ShootProjectile(float InSpeed, float InMaxDistance, const AActor* InTarget, FGenericTeamId InTeamId, FGameplayEffectSpecHandle InHitEffectHandle);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;
	UPROPERTY(Replicated)
	FVector MoveDirection;
	UPROPERTY(Replicated)
	float Speed;
	UPROPERTY()
	const AActor* Target;
	UPROPERTY(EditDefaultsOnly,Category="Gameplay Cue")
	FGameplayTag HitGameplayCueTag;

	FGameplayEffectSpecHandle HitEffectSpecHandle;
	FTimerHandle ShootTimerHandle;

	void MaxDistanceReached();
	void SendLocalGameplayCue(AActor* CueTargetActor, const FHitResult& HitResult);
};
