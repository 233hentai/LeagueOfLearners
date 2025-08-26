// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LobbyWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Widgets/TeamSelectionWidget.h"
#include "Network/LOLNetStatics.h"
#include "Player/LobbyPlayerController.h"
#include "Framework/LOLGameStateBase.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ClearAndPopulateSlotTeamSelectionSlots();
	LobbyPlayerController = GetOwningPlayer<ALobbyPlayerController>();
	ConfigureGameState();
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
	for (const FPlayerSelection& PlayerSelection : PlayerSelections) {
		if (!PlayerSelection.IsValid()) continue;
		TeamSelectionSlots[PlayerSelection.GetPlayerSlot()]->UpdateSlotInfo(PlayerSelection.GetPlayerNickName());
	}
}
