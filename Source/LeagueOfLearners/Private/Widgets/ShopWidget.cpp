// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ShopWidget.h"
#include "Framework/LOLAssetManager.h"
#include "Components/TileView.h"
#include "Widgets/ShopItemWidget.h"
#include "Inventory/InventoryComponent.h"
#include "Widgets/ItemTreeWidget.h"

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	LoadShopItems();
	ShopItemList->OnEntryWidgetGenerated().AddUObject(this, &UShopWidget::ShopItemWidgetGenerated);

	APawn* OwnerPawn = GetOwningPlayerPawn();
	if (OwnerPawn) {
		OwnerInventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>();
	}
}

void UShopWidget::LoadShopItems()
{
	ULOLAssetManager::Get().LoadShopItems(FStreamableDelegate::CreateUObject(this,&UShopWidget::ShopItemLoadFinished));
}

void UShopWidget::ShopItemLoadFinished()
{
	TArray<const UPA_ShopItem*> ShopItems;
	ULOLAssetManager::Get().GetLoadedShopItems(ShopItems);
	for (const UPA_ShopItem* ShopItem : ShopItems) {
		ShopItemList->AddItem(const_cast<UPA_ShopItem*>(ShopItem));
	}
}

void UShopWidget::ShopItemWidgetGenerated(UUserWidget& NewWidget)
{
	UShopItemWidget* ShopItemWidget = Cast<UShopItemWidget>(&NewWidget);
	if (ShopItemWidget) {
		if (OwnerInventoryComponent) {
			ShopItemWidget->OnItemPurchaseIssued.AddUObject(OwnerInventoryComponent, &UInventoryComponent::TryPurchase);
		}
		ShopItemWidget->OnShopItemSelected.AddUObject(this, &UShopWidget::ShowItemCombination);
		ItemsMap.Add(ShopItemWidget->GetShopItem(), ShopItemWidget);
	}
}

void UShopWidget::ShowItemCombination(const UShopItemWidget* ItemWidget)
{
	if (CombinationTree) {
		CombinationTree->DrawFromNode(ItemWidget);
	}
}
