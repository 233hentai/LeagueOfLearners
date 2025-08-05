// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "StatsGauge.generated.h"

/**
 * 
 */
UCLASS()
class UStatsGauge : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* Icon;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AttributeText;
	UPROPERTY(EditAnywhere, Category = "Visual")
	UTexture2D* IconTexture;
	UPROPERTY(EditAnywhere, Category = "Attribute")
	FGameplayAttribute Attribute;
	FNumberFormattingOptions NumberFormattingOptions;

	void SetValue(float NewValue);
	void AttributeChanged(const FOnAttributeChangeData& Data);
};
