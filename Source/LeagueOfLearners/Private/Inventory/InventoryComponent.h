// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryItem.h"
#include "InventoryComponent.generated.h"

class UAbilitySystemComponent;
class UPA_ShopItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAddedDelegate, const UInventoryItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemRemovedDelegate, const FInventoryItemHandle&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemStackCountChangedDelegate, const FInventoryItemHandle&, int);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnItemAbilityCommitedDelegate, const FInventoryItemHandle&, float /*总冷却时长*/,float/*剩余冷却时长*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();
	FOnItemAddedDelegate OnItemAdded;
	FOnItemRemovedDelegate OnItemRemoved;
	FOnItemStackCountChangedDelegate OnItemStackCountChanged;
	FOnItemAbilityCommitedDelegate OnItemAbilityCommited;
	void TryPurchase(const UPA_ShopItem* ItemToPurchase);
	float GetGold() const;
	FORCEINLINE int GetCapacity() const { return Capacity; }
	void ItemSlotChanged(const FInventoryItemHandle& Handle, int NewSlotNumber);
	UInventoryItem* GetInventoryItemFromHandle(const FInventoryItemHandle& Handle) const;
	bool IsAllSlotOccupied() const;
	UInventoryItem* GetAvailableStackForItem(const UPA_ShopItem* Item) const;
	bool IsFullFor(const UPA_ShopItem* Item) const;
	void TryActivateItem(const FInventoryItemHandle& ItemHandle);
	void SellItem(const FInventoryItemHandle& ItemHandle);
	bool FindIngredientForItem(const UPA_ShopItem* Item, TArray<UInventoryItem*>& OutIngredients, const TArray<const UPA_ShopItem*>& IngredientToIgnore = TArray<const UPA_ShopItem*>{});
	UInventoryItem* TryGetItemForShopItem(const UPA_ShopItem* Item) const;
	void TryActivateItemInSlot(int SlotNumber);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int Capacity = 6;
	UPROPERTY()
	UAbilitySystemComponent* OwnerAbilitySystemComponent;
	UPROPERTY()
	TMap<FInventoryItemHandle, UInventoryItem*> InventoryMap;

	void AbilityCommited(class UGameplayAbility* CommitedAbility);

/*************************************************/
/*                     Server                    */
/*************************************************/
private:
	UFUNCTION(Server,Reliable,WithValidation)
	void Server_Purchase(const UPA_ShopItem* ItemToPurchase);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ActivateItem(FInventoryItemHandle ItemHandle);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SellItem(FInventoryItemHandle ItemHandle);

	void GrantItem(const UPA_ShopItem* NewShopItem);
	void ConsumeItem(UInventoryItem* Item);
	void RemoveItem(UInventoryItem* Item);
	bool TryItemCombination(const UPA_ShopItem* NewItem);


/*************************************************/
/*                     Client                    */
/*************************************************/
private:
	UFUNCTION(Client,Reliable)
	void Client_ItemAdded(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* NewItem, FGameplayAbilitySpecHandle GrantedAbilitySpecHandle);
	UFUNCTION(Client,Reliable)
	void Client_ItemRemoved(FInventoryItemHandle ItemHandle);
	UFUNCTION(Client,Reliable)
	void Client_ItemStackChanged(FInventoryItemHandle Handle, int NewCount);
};
