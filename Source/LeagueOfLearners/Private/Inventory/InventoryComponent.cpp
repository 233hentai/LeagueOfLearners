// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/LOLHeroAttributeSet.h"
#include "Inventory/PA_ShopItem.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UInventoryComponent::TryPurchase(const UPA_ShopItem* ItemToPurchase)
{
	if (!OwnerAbilitySystemComponent) return;
	Server_Purchase(ItemToPurchase);
}

float UInventoryComponent::GetGold() const
{
	bool bFound;
	if (OwnerAbilitySystemComponent) {
		float Gold=OwnerAbilitySystemComponent->GetGameplayAttributeValue(ULOLHeroAttributeSet::GetGoldAttribute(), bFound);
		if (bFound) return Gold;
	}
	return 0.f;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	// ...
	
}

void UInventoryComponent::GrantItem(const UPA_ShopItem* NewShopItem)
{
	if (!GetOwner()->HasAuthority()) return;
	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
	FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();
	InventoryItem->InitItem(NewHandle, NewShopItem);
	InventoryMap.Add(NewHandle, InventoryItem);
	OnItemAdded.Broadcast(InventoryItem);
	UE_LOG(LogTemp, Warning, TEXT("Server Adding Shop Item: %s, With ID:%d"),*(InventoryItem->GetShopItem()->GetItemName().ToString()),NewHandle.GetHandleID());
	Client_ItemAdded(NewHandle, NewShopItem);
	InventoryItem->ApplyGASModifications(OwnerAbilitySystemComponent);
}

void UInventoryComponent::Client_ItemAdded_Implementation(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* NewItem)
{
	if (GetOwner()->HasAuthority()) return;
	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
	InventoryItem->InitItem(AssignedHandle, NewItem);
	InventoryMap.Add(AssignedHandle, InventoryItem);
	OnItemAdded.Broadcast(InventoryItem);
	UE_LOG(LogTemp, Warning, TEXT("Client Adding Shop Item: %s, With ID:%d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), AssignedHandle.GetHandleID());
}

void UInventoryComponent::Server_Purchase_Implementation(const UPA_ShopItem* ItemToPurchase)
{
	if (!ItemToPurchase) return;
	if (GetGold() < ItemToPurchase->GetPrice()) return;
	OwnerAbilitySystemComponent->ApplyModToAttribute(ULOLHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());
	GrantItem(ItemToPurchase);
}

bool UInventoryComponent::Server_Purchase_Validate(const UPA_ShopItem* ItemToPurchase)
{
	return true;
}