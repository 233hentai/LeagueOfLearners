// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GAS/LOLGameplayAbilityTypes.h"
#include "GameplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	void ConfigureAbilities(const TMap<ELOLAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities);
	void SwitchShopVisibility();

private:
	UPROPERTY(meta = (BindWidget))
	class UValueGauge* HealthBar;
	UPROPERTY(meta = (BindWidget))
	class UValueGauge* ManaBar;
	UPROPERTY(meta = (BindWidget))
	class UAbilityListView* AbilityListView;
	UPROPERTY(meta = (BindWidget))
	class UStatsGauge* AttackGauge;
	UPROPERTY(meta = (BindWidget))
	class UStatsGauge* ArmorGauge;
	UPROPERTY(meta = (BindWidget))
	class UStatsGauge* MoveSpeedGauge;
	UPROPERTY(meta = (BindWidget))
	class UStatsGauge* IntelligenceGauge;
	UPROPERTY(meta = (BindWidget))
	class UStatsGauge* StrengthGauge;
	UPROPERTY(meta = (BindWidget))
	class UShopWidget* ShopWidget;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* ShopPopupAnimation;
	UPROPERTY()
	class UAbilitySystemComponent* OwnerAbilitySystemComponent;

	void PlayShopPopupAnimation(bool bPlayForward);
	void SetOwningPawnInputEnabled(bool bEnabled);
	void SetShowMouseCursor(bool bShowMouse);
	void SetFocusToGameAndUI();
	void SetFocusToGameOnly();
};
