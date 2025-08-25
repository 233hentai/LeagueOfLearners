// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "CrosshairWidget.generated.h"

/**
 * 
 */
UCLASS()
class UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* CrosshairImage;
	UPROPERTY()
	class UCanvasPanelSlot* CrosshairCanvasPanelSlot;
	UPROPERTY()
	APlayerController* CachedPlayerController;
	UPROPERTY()
	const AActor* AimTarget;
	UPROPERTY(EditDefaultsOnly, Category = "View")
	FLinearColor HasTargetColor = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "View")
	FLinearColor NoTargetColor = FLinearColor::White;

	void CrosshairTagUpdated(const FGameplayTag, int32 NewCount);
	void UpdateCrossHairPosition();
	void TargetUpdated(const struct FGameplayEventData* EventData);
};
