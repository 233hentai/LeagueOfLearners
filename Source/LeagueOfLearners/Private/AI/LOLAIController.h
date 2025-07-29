// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "LOLAIController.generated.h"

/**
 * 
 */
UCLASS()
class ALOLAIController : public AAIController
{
	GENERATED_BODY()
public:
	ALOLAIController();
	virtual void OnPossess(APawn* NewPawn) override;
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	class UAIPerceptionComponent* AIPerceptionComponent;
	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	class UBehaviorTree* BehaviorTree;
	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	FName TargetBlackboardKeyName="Target";

	bool bIsPawnDead = false;
	UFUNCTION()
	void TargetPerceptionUpdated(AActor* TargetActor,FAIStimulus Stimulus);
	UFUNCTION()
	void TargetForgotten(AActor* ForgottenActor);

	const UObject* GetCurrentTarget() const;
	void SetCurrentTarget(AActor* NewTarget);

	AActor* GetNextPerceivedActor() const;

	void ForgetActorIfDead(AActor* ActorToForget);

	void ClearAndDisableAllSenses();
	void EnableAllSenses();

	void PawnDeadTagUpdated(const FGameplayTag Tag,int32 Count);
	void PawnStunTagUpdated(const FGameplayTag Tag, int32 Count);
};
