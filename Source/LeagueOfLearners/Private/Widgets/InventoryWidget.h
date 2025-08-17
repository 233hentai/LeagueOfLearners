// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryItem.h"
#include "InventoryWidget.generated.h"


class UInventoryItemWidget;
class UInventoryContextMenuWidget;
/**
 * 
 */
UCLASS()
class UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;

private:
	UPROPERTY(meta = (BindWidget))
	class UWrapBox* ItemList;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryItemWidget> ItemWidgetClass;
	UPROPERTY()
	class UInventoryComponent* InventoryComponent;
	TArray<UInventoryItemWidget*> ItemWidgets;
	TMap<FInventoryItemHandle, UInventoryItemWidget*> StoredItemEntryWidgets;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryContextMenuWidget> ContextMenuWidgetClass;
	UPROPERTY()
	UInventoryContextMenuWidget* ContextMenuWidget;
	FInventoryItemHandle CurrentFocusedItemHandle;

	void ItemAdded(const UInventoryItem* InventoryItem);
	void ItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount);
	UInventoryItemWidget* GetNextAvailableSlot() const;
	void HandleItemDragDrop(UInventoryItemWidget* DestinationWidget, UInventoryItemWidget* SourceWidget);
	void ItemRemoved(const FInventoryItemHandle& ItemHandle);
	void ItemAbilityCommited(const FInventoryItemHandle& ItemHandle, float CooldownDuration, float CooldownTimeRemaining);

	UFUNCTION()
	void UseFocusedItem();
	UFUNCTION()
	void SellFocusedItem();
	void SpawnContextMenu();
	void SetContextMenuVisible(bool bContextMenuVisible);
	void SwitchContextMenu(const FInventoryItemHandle& ItemHandle);
	void ClearContextMenu();
};
