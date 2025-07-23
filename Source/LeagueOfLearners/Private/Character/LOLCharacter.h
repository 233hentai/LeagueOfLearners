// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h" 
#include "GenericTeamAgentInterface.h"
#include "LOLCharacter.generated.h"

UCLASS()
class ALOLCharacter : public ACharacter, public IAbilitySystemInterface,public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALOLCharacter();
	void ServerSideInit();
	void ClientSideInit();
	bool IsControlledByLocalPlayer() const;
	virtual void PossessedBy(AController* NewController) override;//仅在服务器端调用

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

/*******************************************************************/
/*                       Gameplay Ability                          */
/*******************************************************************/
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	void BindGASChangeDelegates();
	void DeathTagUpdated(const FGameplayTag Tag,int32 NewCount);
	UPROPERTY(VisibleDefaultsOnly, Category = "UI")
	class ULOLAbilitySystemComponent* LOLAbilitySystemComponent;
	UPROPERTY()
	class ULOLAttributeSet* LOLAttributeSet;

/*******************************************************************/
/*                              UI                                 */
/*******************************************************************/
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	class UWidgetComponent* OverHeadWidgetComponent;
	void ConfigureOverHeadStatsWidget();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HeadStatsGaugeVisibilityCheckGap = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HeadStatsGaugeVisibilityRangeSquared = 1000000.f;//距离的平方而不是距离，少一次开方运算
	FTimerHandle OverHeadStatsGaugeVisibilityHandle;
	void UpdateHeadStatsGaugeVisibility();
	void SetStatusGaugeEnabled(bool bIsEnabled);

/*******************************************************************/
/*                       Death and Respawn                         */
/*******************************************************************/
private:
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathMontage;
	void PlayDeathMontage();

	void StartDeathSequence(); 
	void Respawn();

	virtual void OnDead();
	virtual void OnRespawn();

	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float DeathMontageFinishTimeShift=-0.8f;
	FTimerHandle DeathMontageTimerHandle;
	FTransform MeshRelativeTransform;
	void DeathMontageFinished();
	void SetRagDollEnabled(bool bIsEnabled);

/*******************************************************************/
/*                               Team                              */
/*******************************************************************/
public:
	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override;

private:
	UPROPERTY(Replicated)
	FGenericTeamId TeamID;
};
