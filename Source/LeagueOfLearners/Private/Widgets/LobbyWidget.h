// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/PlayerInfoTypes.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;


private:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MainSwitcher;
	UPROPERTY(meta = (BindWidget))
	class UWidget* TeamSelectionRoot;
	UPROPERTY(meta = (BindWidget))
	class UButton* StartHeroSelectionButton;
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* TeamSelectionSlotGridPanel;
	UPROPERTY(meta = (BindWidget))
	class UTileView* HeroSelectionTileView;


	UPROPERTY(EditDefaultsOnly, Category = "TeamSelection")
	TSubclassOf<class UTeamSelectionWidget> TeamSelectionWidgetClass;
	UPROPERTY()
	TArray<class UTeamSelectionWidget*> TeamSelectionSlots;
	UPROPERTY()
	class ALOLPlayerState* LOLPlayerState;

	void ClearAndPopulateSlotTeamSelectionSlots();
	void SlotSelected(uint8 NewSlotId);

	class ALobbyPlayerController* LobbyPlayerController;

	UPROPERTY()
	class ALOLGameStateBase* LOLGameStateBase;
	FTimerHandle ConfigureGameStateTimerHandle;

	void ConfigureGameState();
	void UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections);

	UFUNCTION()
	void StartHeroSelectionButtonCliked();


	UPROPERTY(meta = (BindWidget))
	class UWidget* HeroSelectionRoot;
	void SwitchToHeroSelection();
	void HeroDefinitionLoaded();
	void CharacterSelected(UObject* SelectedObject);


	UPROPERTY(EditDefaultsOnly,Category="Character Display")
	TSubclassOf<class ACharacterDisplay> CharacterDisplayClass;
	UPROPERTY()
	class ACharacterDisplay* CharacterDisplay;

	void SpawnCharacterDisplay();
	void UpdateCharacterDisplay(const FPlayerSelection& PlayerSelection);


	UPROPERTY(meta = (BindWidget))
	class UAbilityListView* AbilityListView;
	UPROPERTY(meta = (BindWidget))
	class UPlayerTeamLayoutWidget* PlayerTeamLayoutWidget;


	UPROPERTY(meta = (BindWidget))
	class UButton* StartMatchButton;

	UFUNCTION()
	void StartMatchButtonClicked();
};
