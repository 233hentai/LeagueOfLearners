// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GenericTeamAgentInterface.h"
#include "TA_Blackhole.generated.h"

/**
 * 
 */
UCLASS()
class ATA_Blackhole : public AGameplayAbilityTargetActor, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	ATA_Blackhole();
	void ConfigureBlackhole(float InBlackholeRange, float InPullSpeed, float InBlackholeDuration, const FGenericTeamId& InTeamId);
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void Tick(float DeltaTime) override;
	virtual void ConfirmTargetingAndContinue() override;
	virtual void CancelTargeting() override;


private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	class USceneComponent* RootComp;
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	class USphereComponent* DetectionSphereComponent;
	UPROPERTY(VisibleDefaultsOnly, Category = "Component")
	class UParticleSystemComponent* VFXComponent;
	UPROPERTY(Replicated)
	FGenericTeamId TeamId;

	float PullSpeed;
	float BlackholeDuration;
	UPROPERTY(ReplicatedUsing = OnRep_BlackholeRange)
	float BlackholeRange;
	FTimerHandle BlackholeDurationTimerHandle;


	UFUNCTION()
	void ActorEnterBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void ActorLeftBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnRep_BlackholeRange();

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	FName BlackholeVFXOriginVariableName = "Origin";
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	class UNiagaraSystem* BlackholeLinkVFX;
	TMap<AActor*, class UNiagaraComponent*> ActorsInRangeMap;

	void TryAddATarget(AActor* OtherTarget);
	void TryRemoveATarget(AActor* OtherTarget);
	void StopBlackhole();
};
