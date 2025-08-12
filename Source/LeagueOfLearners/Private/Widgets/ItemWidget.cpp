// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemWidget.h"
#include "Components/Image.h"
#include "Widgets/ItemToolTip.h"

void UItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
}

void UItemWidget::SetIcon(UTexture2D* IconTexture)
{
	ItemIcon->SetBrushFromTexture(IconTexture);
}

UItemToolTip* UItemWidget::SetToolTipWidget(const UPA_ShopItem* Item)
{
	if (!Item) return nullptr;
	if (GetOwningPlayer() && ItemToolTipClass) {
		UItemToolTip* ItemToolTip = CreateWidget<UItemToolTip>(GetOwningPlayer(), ItemToolTipClass);
		if (ItemToolTip) {
			ItemToolTip->SetItem(Item);
			SetToolTip(ItemToolTip);
		}
		return ItemToolTip;
	}
	return nullptr;
}

FReply UItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton)) {
		return FReply::Handled().SetUserFocus(TakeWidget());
	}
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)) {
		return FReply::Handled().SetUserFocus(TakeWidget()).DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return SuperReply;
}

FReply UItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	if (HasAnyUserFocus()) {
		if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton) {
			RightMouseButtonClicked();
			return FReply::Handled();
		}
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) {
			LeftMouseButtonClicked();
			return FReply::Handled();
		}
	}
	return SuperReply;
}

void UItemWidget::RightMouseButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("Right Mouse Button Clicked"));
}

void UItemWidget::LeftMouseButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("Left Mouse Button Clicked"));
}
