// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryWidget.h"
#include "Widgets/InventoryItemWidget.h"
#include "Inventory/InventoryComponent.h"
#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "Widgets/InventoryContextMenuWidget.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"


void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (APawn* OwnerPawn = GetOwningPlayerPawn()) {
		InventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>();
		if (InventoryComponent) {
			InventoryComponent->OnItemAdded.AddUObject(this, &UInventoryWidget::ItemAdded);
			InventoryComponent->OnItemRemoved.AddUObject(this, &UInventoryWidget::ItemRemoved);
			InventoryComponent->OnItemStackCountChanged.AddUObject(this, &UInventoryWidget::ItemStackCountChanged);
			int Capacity = InventoryComponent->GetCapacity();
			ItemList->ClearChildren();
			for (int i = 0; i < Capacity; ++i) {
				UInventoryItemWidget* NewEmptyWidget = CreateWidget<UInventoryItemWidget>(GetOwningPlayer(), ItemWidgetClass);
				if (NewEmptyWidget) {
					NewEmptyWidget->SetSlotNumber(i);
					UWrapBoxSlot* NewItemSlot = ItemList->AddChildToWrapBox(NewEmptyWidget);
					NewItemSlot->SetPadding(FMargin(2.f));
					ItemWidgets.Add(NewEmptyWidget);
					NewEmptyWidget->OnInventoryItemDropped.AddUObject(this, &UInventoryWidget::HandleItemDragDrop);
					NewEmptyWidget->OnLeftButtonClick.AddUObject(InventoryComponent, &UInventoryComponent::TryActivateItem);
					NewEmptyWidget->OnRightButtonClick.AddUObject(this, &UInventoryWidget::SwitchContextMenu);
				}
			}
			SpawnContextMenu();
		}
	}
}

void UInventoryWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);
	if (!NewWidgetPath.ContainsWidget(ContextMenuWidget->GetCachedWidget().Get())) {
		ClearContextMenu();
	}
}

void UInventoryWidget::ItemAdded(const UInventoryItem* InventoryItem)
{
	if (!InventoryItem) return;
	if (UInventoryItemWidget* NextAvailableSlot = GetNextAvailableSlot()) {
		NextAvailableSlot->UpdateInventoryItem(InventoryItem);
		StoredItemEntryWidgets.Add(InventoryItem->GetHandle(), NextAvailableSlot);
		if (InventoryComponent) {
			InventoryComponent->ItemSlotChanged(InventoryItem->GetHandle(), NextAvailableSlot->GetSlotNumber());
		}
	}
}

void UInventoryWidget::ItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount)
{
	UInventoryItemWidget** FoundWidget = StoredItemEntryWidgets.Find(Handle);
	if (FoundWidget) {
		(*FoundWidget)->UpdateStackCount();
	}
}

UInventoryItemWidget* UInventoryWidget::GetNextAvailableSlot() const
{
	for (UInventoryItemWidget* Widget : ItemWidgets) {
		if (Widget->IsEmpty()) {
			return Widget;
		}
	}
	return nullptr;
}

void UInventoryWidget::HandleItemDragDrop(UInventoryItemWidget* DestinationWidget, UInventoryItemWidget* SourceWidget)
{
	const UInventoryItem* SourceItem = SourceWidget->GetInventoryItem();
	const UInventoryItem* DestinationItem = DestinationWidget->GetInventoryItem();
	DestinationWidget->UpdateInventoryItem(SourceItem);
	SourceWidget->UpdateInventoryItem(DestinationItem);
	StoredItemEntryWidgets[DestinationWidget->GetItemHandle()] = DestinationWidget;
	if (InventoryComponent) {
		InventoryComponent->ItemSlotChanged(DestinationWidget->GetItemHandle(), DestinationWidget->GetSlotNumber());
	}
	if (!SourceWidget->IsEmpty()) {
		StoredItemEntryWidgets[SourceWidget->GetItemHandle()] = SourceWidget;
		if (InventoryComponent) {
			InventoryComponent->ItemSlotChanged(SourceWidget->GetItemHandle(), SourceWidget->GetSlotNumber());
		}
	}
	
}

void UInventoryWidget::ItemRemoved(const FInventoryItemHandle& ItemHandle)
{
	UInventoryItemWidget** FoundItemWidget = StoredItemEntryWidgets.Find(ItemHandle);
	if (FoundItemWidget && *FoundItemWidget) {
		(*FoundItemWidget)->EmptySlot();
		StoredItemEntryWidgets.Remove(ItemHandle);
	}
}

void UInventoryWidget::SpawnContextMenu()
{
	if (!ContextMenuWidgetClass) return;
	ContextMenuWidget = CreateWidget<UInventoryContextMenuWidget>(this, ContextMenuWidgetClass);
	if (ContextMenuWidget) {
		ContextMenuWidget->GetUseButtonClickedEvent().AddDynamic(this, &UInventoryWidget::UseFocusedItem);
		ContextMenuWidget->GetSellButtonClickedEvent().AddDynamic(this, &UInventoryWidget::SellFocusedItem);
		ContextMenuWidget->AddToViewport(1);
		SetContextMenuVisible(false);
	}
}

void UInventoryWidget::UseFocusedItem()
{
	//UE_LOG(LogTemp, Warning, TEXT("Using Item"));
	InventoryComponent->TryActivateItem(CurrentFocusedItemHandle);
	SetContextMenuVisible(false);
}

void UInventoryWidget::SellFocusedItem()
{
	//UE_LOG(LogTemp, Warning, TEXT("Selling Item"));
	InventoryComponent->SellItem(CurrentFocusedItemHandle);
	SetContextMenuVisible(false);
}

void UInventoryWidget::SetContextMenuVisible(bool bContextMenuVisible)
{
	if (ContextMenuWidget) {
		ContextMenuWidget->SetVisibility(bContextMenuVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UInventoryWidget::SwitchContextMenu(const FInventoryItemHandle& ItemHandle)
{
	if (CurrentFocusedItemHandle == ItemHandle) {
		ClearContextMenu();
		return;
	}
	CurrentFocusedItemHandle = ItemHandle;
	UInventoryItemWidget** ItemWidget = StoredItemEntryWidgets.Find(ItemHandle);
	if (!ItemWidget||!(*ItemWidget)) return;
	SetContextMenuVisible(true);
	FVector2D ItemAbsPosition = (*ItemWidget)->GetCachedGeometry().GetAbsolutePositionAtCoordinates(FVector2D{ 1.f,0.5f });
	FVector2D ItemWidgetPixelPos, ItemWidgetViewportPos;
	USlateBlueprintLibrary::AbsoluteToViewport(this, ItemAbsPosition, ItemWidgetPixelPos, ItemWidgetViewportPos);
	APlayerController* OwningPlayerController = GetOwningPlayer();
	if (OwningPlayerController) {
		int ViewportSzieX, ViewportSzieY;
		OwningPlayerController->GetViewportSize(ViewportSzieX, ViewportSzieY);
		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		int Overshoot = ItemWidgetPixelPos.Y + ContextMenuWidget->GetDesiredSize().Y * Scale - ViewportSzieY;
		if (Overshoot > 0) {
			ItemWidgetPixelPos.Y -= Overshoot;
		}
	}
	ContextMenuWidget->SetPositionInViewport(ItemWidgetPixelPos);
}

void UInventoryWidget::ClearContextMenu()
{
	ContextMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	CurrentFocusedItemHandle = FInventoryItemHandle::InvalidHandle();
}

