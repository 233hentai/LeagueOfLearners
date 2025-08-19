// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StormCore.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGoalReachedDelegate, AActor* /*view target*/, int /*winner team*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTeamInluencerUpdatedDelegate, int /*team one influencer count*/, int /*team two influencer count*/);

UCLASS()
class AStormCore : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	FOnGoalReachedDelegate OnGoalReached;
	FOnTeamInluencerUpdatedDelegate OnTeamInluencerUpdated;
	AStormCore();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	float GetProgress() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	int TeamOneInfluencerCount = 0;
	int TeamTwoInfluencerCount = 0;
	float TeamWeight = 0.f;
	float CoreCaptureSpeed = 0.f;
	float WholeLength = 0.f;

	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	class USphereComponent* InfluenceRange;
	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float MaxMoveSpeed = 500.f;
	UPROPERTY(EditAnywhere, Category = "Team")
	AActor* TeamOneGoal;
	UPROPERTY(EditAnywhere, Category = "Team")
	AActor* TeamTwoGoal;
	UPROPERTY(EditAnywhere, Category = "Team")
	AActor* TeamOneCore;
	UPROPERTY(EditAnywhere, Category = "Team")
	AActor* TeamTwoCore;
	UPROPERTY(ReplicatedUsing = OnRepCoreToCapture)
	AActor* CoreToCapture;
	UPROPERTY()
	class AAIController* OwnerAIC;
	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	class UDecalComponent* GroundDecalComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Move")
	float InfluenceRadius = 1000.f;
	UPROPERTY(VisibleDefaultsOnly, Category = "Detection")
	class UCameraComponent* ViewCamera;
	UPROPERTY(EditDefaultsOnly, Category = "Move")
	UAnimMontage* ExpandMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Move")
	UAnimMontage* CaptureMontage;
	UFUNCTION()
	void NewInfluencerInRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void InfluencerLeftRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnRepCoreToCapture();

	void UpdateTeamWeight();
	void UpdateGoal();
	void GoalReached(int WinnerTeam);
	void CaptureCore();
	void ExpandFinished();
};
