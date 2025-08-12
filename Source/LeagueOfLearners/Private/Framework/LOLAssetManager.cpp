// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LOLAssetManager.h"

ULOLAssetManager& ULOLAssetManager::Get()
{
	ULOLAssetManager* Singleton = Cast<ULOLAssetManager>(GEngine->AssetManager.Get());
	if (Singleton) return *Singleton;
	UE_LOG(LogLoad, Fatal, TEXT("Asset Manager needs to be of the type ULOLAssetManager"));
	return (*NewObject<ULOLAssetManager>());
}

void ULOLAssetManager::LoadShopItems(const FStreamableDelegate& LoadFinishedCallback)
{
	LoadPrimaryAssetsWithType(UPA_ShopItem::GetShopItemAssetType(), TArray<FName>(), FStreamableDelegate::CreateUObject(this, &ULOLAssetManager::ShopItemLoadFinished, LoadFinishedCallback));
}

bool ULOLAssetManager::GetLoadedShopItems(TArray<const UPA_ShopItem*>& OutItems) const
{
	TArray<UObject*> LoadedObjects;
	bool bLoaded = GetPrimaryAssetObjectList(UPA_ShopItem::GetShopItemAssetType(), LoadedObjects);
	if (bLoaded) {
		for (UObject* LoadedObject : LoadedObjects) {
			OutItems.Add(Cast<UPA_ShopItem>(LoadedObject));
		}
	}
	return bLoaded;
}

void ULOLAssetManager::ShopItemLoadFinished(FStreamableDelegate Callback)
{
	Callback.ExecuteIfBound();
	BuildItemMaps();
}

void ULOLAssetManager::BuildItemMaps()
{
	TArray<const UPA_ShopItem*> LoadItems;
	if (GetLoadedShopItems(LoadItems)) {
		for (const UPA_ShopItem* Item : LoadItems) {
			if (Item->GetIngredientItems().Num() == 0) continue;
			TArray<const UPA_ShopItem*> Items;
			for (const TSoftObjectPtr<UPA_ShopItem>& Ingredient : Item->GetIngredientItems()) {
				UPA_ShopItem* IngredientItem = Ingredient.LoadSynchronous();
				Items.Add(IngredientItem);
				AddToCombinationMap(IngredientItem,Item);
			}
			IngredientMap.Add(Item, FItemCollection{ Items });
		}
	}
}

void ULOLAssetManager::AddToCombinationMap(const UPA_ShopItem* Ingredient, const UPA_ShopItem* CombinationItem)
{
	FItemCollection* Combinations = CombinationMap.Find(Ingredient);
	if (Combinations) {
		if (!Combinations->Contains(CombinationItem)) {
			Combinations->AddItem(CombinationItem);
		}
	}
	else {
		CombinationMap.Add(Ingredient, FItemCollection{ TArray<const UPA_ShopItem*>{CombinationItem} });
	}
}
