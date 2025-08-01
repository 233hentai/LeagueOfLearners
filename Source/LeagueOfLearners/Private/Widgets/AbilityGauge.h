// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "AbilityGauge.generated.h"

USTRUCT(BlueprintType)
struct FAbilityWidgetData :public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<class UGameplayAbility> AbilityClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AbilityName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};

/**
 * 
 */
UCLASS()
class UAbilityGauge : public UUserWidget,public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	void ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData);
private:
	UPROPERTY(meta = (BindWidget))
	class UImage* Icon;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CooldownCounterText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CooldownDurationText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CostText;
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName IconMaterialParamName = "Icon";
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CooldownPercentParamName = "Percent";
	UPROPERTY()
	class UGameplayAbility* AbilityClassDefaultObject;

	void AbilityCommited(UGameplayAbility* Ability);
	void StartCooldown(float CooldownTimeRemaining,float CooldownDuration);
	float CachedCooldownTimeRemaining;
	float CachedCooldownDuration;
	FTimerHandle CooldownTimerHandle;
	FTimerHandle CooldownTimerUpdateHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownUpdateInterval = 0.1f;
	FNumberFormattingOptions WholeNumberFormattingOptions;
	FNumberFormattingOptions TwoDigitNumberFormattingOptions;
	void CooldownFinished();
	void UpdateCooldown();
};
