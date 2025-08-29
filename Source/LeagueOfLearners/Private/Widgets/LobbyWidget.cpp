// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LobbyWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Button.h"
#include "Widgets/TeamSelectionWidget.h"
#include "Network/LOLNetStatics.h"
#include "Player/LobbyPlayerController.h"
#include "Framework/LOLGameStateBase.h"
#include "Components/WidgetSwitcher.h"
#include "Framework/LOLAssetManager.h"
#include "Character/PA_HeroDefinition.h"
#include "Components/TileView.h"
#include "Player/LOLPlayerState.h"
#include "Widgets/CharacterEntryWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Widgets/CharacterDisplay.h"
#include "Widgets/AbilityListView.h"
#include "Widgets/PlayerTeamLayoutWidget.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ClearAndPopulateSlotTeamSelectionSlots();
	ConfigureGameState();
	LobbyPlayerController = GetOwningPlayer<ALobbyPlayerController>();
	if (LobbyPlayerController) {
		LobbyPlayerController->OnSwitchToHeroSelection.BindUObject(this, &ULobbyWidget::SwitchToHeroSelection);
	}
	StartHeroSelectionButton->SetIsEnabled(false);
	StartHeroSelectionButton->OnClicked.AddDynamic(this, &ULobbyWidget::StartHeroSelectionButtonCliked);

	StartMatchButton->SetIsEnabled(false);
	StartMatchButton->OnClicked.AddDynamic(this, &ULobbyWidget::StartMatchButtonClicked);

	ULOLAssetManager::Get().LoadHeroDefinitions(FStreamableDelegate::CreateUObject(this,&ULobbyWidget::HeroDefinitionLoaded));
	if (HeroSelectionTileView) {
		HeroSelectionTileView->OnItemSelectionChanged().AddUObject(this, &ULobbyWidget::CharacterSelected);
	}

	SpawnCharacterDisplay();
}

void ULobbyWidget::ClearAndPopulateSlotTeamSelectionSlots()
{
	TeamSelectionSlotGridPanel->ClearChildren();
	for (int i = 0; i < ULOLNetStatics::GetPlayerCountPerTeam() * 2; ++i) {
		UTeamSelectionWidget* NewSelectionSlot = CreateWidget<UTeamSelectionWidget>(this, TeamSelectionWidgetClass);
		if (NewSelectionSlot) {
			NewSelectionSlot->SetSlotId(i);
			UUniformGridSlot* NewGridSlot = TeamSelectionSlotGridPanel->AddChildToUniformGrid(NewSelectionSlot);
			if (NewGridSlot) {
				int Row = i % ULOLNetStatics::GetPlayerCountPerTeam();
				int Col = i < ULOLNetStatics::GetPlayerCountPerTeam() ? 0 : 1;
				NewGridSlot->SetRow(Row);
				NewGridSlot->SetColumn(Col);
			}
			NewSelectionSlot->OnSlotClicked.AddUObject(this, &ULobbyWidget::SlotSelected);
			TeamSelectionSlots.Add(NewSelectionSlot);
		}
	}
}

void ULobbyWidget::SlotSelected(uint8 NewSlotId)
{
	if (LobbyPlayerController) {
		LobbyPlayerController->Server_RequestSelectionChange(NewSlotId);
	}
}

void ULobbyWidget::ConfigureGameState()
{
	UWorld* World = GetWorld();
	if (!World) return;
	LOLGameStateBase = World->GetGameState<ALOLGameStateBase>();
	if (!LOLGameStateBase) {
		World->GetTimerManager().SetTimer(ConfigureGameStateTimerHandle, this, &ULobbyWidget::ConfigureGameState, 1.f);
	}
	else {
		LOLGameStateBase->OnPlayerSelectionUpdated.AddUObject(this, &ULobbyWidget::UpdatePlayerSelectionDisplay);
		UpdatePlayerSelectionDisplay(LOLGameStateBase->GetPlayerSelection());
	}
}

void ULobbyWidget::UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelections)
{
	for (UTeamSelectionWidget* SelectionSlot : TeamSelectionSlots) {
		SelectionSlot->UpdateSlotInfo("Empty");
	}

	for (UUserWidget* CharacterEntryAsWidget : HeroSelectionTileView->GetDisplayedEntryWidgets()) {
		if (UCharacterEntryWidget* CharacterEntryWidget = Cast<UCharacterEntryWidget>(CharacterEntryAsWidget)) {
			CharacterEntryWidget->SetSelected(false);
		}
	}

	for (const FPlayerSelection& PlayerSelection : PlayerSelections) {
		if (!PlayerSelection.IsValid()) continue;
		TeamSelectionSlots[PlayerSelection.GetPlayerSlot()]->UpdateSlotInfo(PlayerSelection.GetPlayerNickName());
		UCharacterEntryWidget* SelectedEntry = HeroSelectionTileView->GetEntryWidgetFromItem<UCharacterEntryWidget>(PlayerSelection.GetHeroDefinition());
		if(SelectedEntry)
		{
			SelectedEntry->SetSelected(true);
		}

		if (PlayerSelection.IsForPlayer(GetOwningPlayerState())) {
			UpdateCharacterDisplay(PlayerSelection);
		}
	}

	if (LOLGameStateBase) {
		StartHeroSelectionButton->SetIsEnabled(LOLGameStateBase->CanStartHeroSelection());
		StartMatchButton->SetIsEnabled(LOLGameStateBase->CanStartMatch());
	}

	if (PlayerTeamLayoutWidget) {
		PlayerTeamLayoutWidget->UpdatePlayerSelection(PlayerSelections);
	}
}

void ULobbyWidget::StartHeroSelectionButtonCliked()
{
	if (LobbyPlayerController) {
		LobbyPlayerController->Server_StartHeroSelection();
	}
}

void ULobbyWidget::SwitchToHeroSelection()
{
	MainSwitcher->SetActiveWidget(HeroSelectionRoot);
}

void ULobbyWidget::HeroDefinitionLoaded()
{
	TArray<UPA_HeroDefinition*> LoadedHeroDefinitions;
	if (ULOLAssetManager::Get().GetLoadedHeroDefinitions(LoadedHeroDefinitions)) {
		HeroSelectionTileView->SetListItems(LoadedHeroDefinitions);
	}
}

void ULobbyWidget::CharacterSelected(UObject* SelectedObject)
{
	if (!LOLPlayerState) {
		LOLPlayerState = GetOwningPlayerState<ALOLPlayerState>();
	}
	if (!LOLPlayerState) return;

	if (const UPA_HeroDefinition* HeroDefinition = Cast<UPA_HeroDefinition>(SelectedObject)) {
		LOLPlayerState->Server_SetSelectedHeroDefinition(HeroDefinition);
	}
}

void ULobbyWidget::SpawnCharacterDisplay()
{
	if (CharacterDisplay) return;
	if (!CharacterDisplayClass) return;

	FTransform CharacterDisplayTransform = FTransform::Identity;
	AActor* PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass());
	if (PlayerStart) {
		CharacterDisplayTransform = PlayerStart->GetActorTransform();
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CharacterDisplay = GetWorld()->SpawnActor<ACharacterDisplay>(CharacterDisplayClass, CharacterDisplayTransform, SpawnParams);
	GetOwningPlayer()->SetViewTarget(CharacterDisplay);
}

void ULobbyWidget::UpdateCharacterDisplay(const FPlayerSelection& PlayerSelection)
{
	if (!PlayerSelection.GetHeroDefinition()) return;
	CharacterDisplay->ConfigureWithHeroDefinition(PlayerSelection.GetHeroDefinition());
	AbilityListView->ClearListItems();
	const TMap<ELOLAbilityInputID, TSubclassOf<UGameplayAbility>>* Abilities = PlayerSelection.GetHeroDefinition()->GetAbilities();
	if (Abilities) {
		AbilityListView->ConfigureAbilities(*Abilities);
	}
}

void ULobbyWidget::StartMatchButtonClicked()
{
	if (LobbyPlayerController) {
		LobbyPlayerController->Server_RequestStartMatch();
	}
}
