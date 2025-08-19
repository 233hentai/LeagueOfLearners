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
	void ShowGameplayMenu();
	void SetGameplayMenuTitle(const FString& NewTitle);
	UFUNCTION()
	void SwitchGameplayMenu();

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
	UPROPERTY(meta = (BindWidget))
	class UInventoryWidget* InventoryWidget;
	UPROPERTY(meta = (BindWidget))
	class USkeletalMeshRenderWidget* HeadshotWidget;
	UPROPERTY(meta = (BindWidget))
	class UMatchStatWidget* MatchStatWidget;
	UPROPERTY(meta = (BindWidget))
	class UGameplayMenu* GameplayMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MainSwitcher;
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* GameplayWidgetRootPanel;
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* GameplayMenuRootPanel;

	void PlayShopPopupAnimation(bool bPlayForward);
	void SetOwningPawnInputEnabled(bool bEnabled);
	void SetShowMouseCursor(bool bShowMouse);
	void SetFocusToGameAndUI();
	void SetFocusToGameOnly();
};
