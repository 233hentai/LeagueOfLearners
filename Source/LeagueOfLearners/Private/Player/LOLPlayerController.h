// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "LOLPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ALOLPlayerController : public APlayerController,public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	//仅服务器端调用
	void OnPossess(APawn* NewPawn) override;

	//仅客户端调用
	void AcknowledgePossession(APawn* NewPawn) override;

	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void SetupInputComponent() override;

	void MatchFinished(AActor* ViewTarget, int WinnerTeam);

private:
	UPROPERTY()
	class ALOLPlayerCharacter* LOLPlayerCharacter;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UGameplayWidget> GameplayWidgetClass;
	UPROPERTY()
	class UGameplayWidget* GameplayWidget;
	UPROPERTY(Replicated)
	FGenericTeamId TeamID;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* UIInputMapping;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* ShopSwitchInputAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* SwitchGameplayMenuAction;
	UPROPERTY(EditDefaultsOnly, Category = "View")
	float MatchFinishViewBlendTimeDuration = 2.f;

	void SpawnGameplayWidget();
	void SwitchShopVisibility();
	void SwitchGameplayMenu();
	void ShowMatchResult();

	UFUNCTION(Client,Reliable)
	void Client_MatchFinished(AActor* ViewTarget, int WinnerTeam);
};
