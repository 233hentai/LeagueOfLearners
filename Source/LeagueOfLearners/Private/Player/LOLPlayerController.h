// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LOLPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ALOLPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	//���������˵���
	void OnPossess(APawn* NewPawn) override;

	//���ͻ��˵���
	void AcknowledgePossession(APawn* NewPawn) override;

private:
	void SpawnGameplayWidget();

	UPROPERTY()
	class ALOLPlayerCharacter* LOLPlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	class UGameplayWidget* GameplayWidget;
};
