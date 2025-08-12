// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/ItemWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ShopItemWidget.generated.h"

class UPA_ShopItem;
class UShopItemWidget;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPurchaseIssued, const UPA_ShopItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UShopItemWidget*);
/**
 * 
 */
UCLASS()
class UShopItemWidget : public UItemWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	FOnItemPurchaseIssued OnItemPurchaseIssued;
	FOnShopItemSelected OnShopItemSelected;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	FORCEINLINE const UPA_ShopItem* GetShopItem() { return ShopItem; }

private:
	const UPA_ShopItem* ShopItem;

	virtual void RightMouseButtonClicked() override;
	virtual void LeftMouseButtonClicked() override;
};
