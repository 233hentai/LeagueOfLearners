// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryItemDragDropOperation.h"
#include "Widgets/ItemWidget.h"
#include "Widgets/InventoryItemWidget.h"


void UInventoryItemDragDropOperation::SetDraggedItem(UInventoryItemWidget* DraggedItem)
{
	Payload = DraggedItem;
	if (DragVisualClass) {
		UItemWidget* DragItemWidget = CreateWidget<UItemWidget>(GetWorld(), DragVisualClass);
		if (DragItemWidget) {
			DragItemWidget->SetIcon(DraggedItem->GetIconTexture());
			DefaultDragVisual = DragItemWidget;
		}
	}
}
