// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchStatWidget.generated.h"

/**
 * 
 */
UCLASS()
class UMatchStatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TeamOneCountText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TeamTwoCountText;
	UPROPERTY(meta = (BindWidget))
	class UImage* ProgressImage;
	UPROPERTY(EditDefaultsOnly, Category = "Match Stat")
	FName ProgressDynamicMaterialParamName = "Progress";
	UPROPERTY()
	class AStormCore* StormCore;

	UPROPERTY(EditDefaultsOnly, Category = "Match Stat")
	float UpdateProgressInterval = 0.5;
	FTimerHandle UpdateProgressTimerHandle;

	void UpdateTeamInfluence(int TeamOneCount, int TeamTwoCount);
	void MatchFinished(AActor* ViewTarget, int WinnerTeam);
	void UpdateProgress();
};
