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

void UInventoryComponent::ItemSlotChanged(const FInventoryItemHandle& Handle, int NewSlotNumber)
{
	if (UInventoryItem* FoundItem = GetInventoryItemFromHandle(Handle)) {
		FoundItem->SetSlot(NewSlotNumber);
	}
}

UInventoryItem* UInventoryComponent::GetInventoryItemFromHandle(const FInventoryItemHandle& Handle) const
{
	UInventoryItem* const* FoundItem = InventoryMap.Find(Handle);
	if (FoundItem) {
		return *FoundItem;
	}
	return nullptr;
}

bool UInventoryComponent::IsAllSlotOccupied() const
{
	return InventoryMap.Num() >= GetCapacity();
}

UInventoryItem* UInventoryComponent::GetAvailableStackForItem(const UPA_ShopItem* Item) const
{
	if (!Item->IsStackable()) return nullptr;
	for (const TPair<FInventoryItemHandle, UInventoryItem*> ItemPair : InventoryMap) {
		if (ItemPair.Value && ItemPair.Value->IsForItem(Item) && !ItemPair.Value->IsStackFull()) {
			return ItemPair.Value;
		}
	}
	return nullptr;
}

bool UInventoryComponent::IsFullFor(const UPA_ShopItem* Item) const
{
	if (!Item) return false;
	if (IsAllSlotOccupied()) {
		return GetAvailableStackForItem(Item) == nullptr;
	}
	return false;
}

void UInventoryComponent::TryActivateItem(const FInventoryItemHandle& ItemHandle)
{
	UInventoryItem* InventoryItem = GetInventoryItemFromHandle(ItemHandle);
	if (!InventoryItem) return;
	Server_ActivateItem(ItemHandle);
}

void UInventoryComponent::SellItem(const FInventoryItemHandle& ItemHandle)
{
	Server_SellItem(ItemHandle);
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	// ...
	
}

void UInventoryComponent::Server_ActivateItem_Implementation(FInventoryItemHandle ItemHandle)
{
	UInventoryItem* InventoryItem = GetInventoryItemFromHandle(ItemHandle);
	if (!InventoryItem) return;
	InventoryItem->TryActivateGrantedAbility(OwnerAbilitySystemComponent);
	const UPA_ShopItem* ShopItem = InventoryItem->GetShopItem();
	if (ShopItem->CanBeConsumed()) {
		ConsumeItem(InventoryItem);
	}
}

bool UInventoryComponent::Server_ActivateItem_Validate(FInventoryItemHandle ItemHandle)
{
	return true;
}

void UInventoryComponent::Server_SellItem_Implementation(FInventoryItemHandle ItemHandle)
{
	UInventoryItem* InventoryItem = GetInventoryItemFromHandle(ItemHandle);
	if (!InventoryItem || !InventoryItem->IsValid()) return;
	float SellPrice = InventoryItem->GetShopItem()->GetSellPrice();
	OwnerAbilitySystemComponent->ApplyModToAttribute(ULOLHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, SellPrice*InventoryItem->GetStackCount());
	RemoveItem(InventoryItem);
}

bool UInventoryComponent::Server_SellItem_Validate(FInventoryItemHandle ItemHandle)
{
	return true;
}

void UInventoryComponent::GrantItem(const UPA_ShopItem* NewShopItem)
{
	if (!GetOwner()->HasAuthority()) return;

	if (UInventoryItem* StackItem = GetAvailableStackForItem(NewShopItem)) {
		StackItem->AddStackCount();
		OnItemStackCountChanged.Broadcast(StackItem->GetHandle(), StackItem->GetStackCount());
		Client_ItemStackChanged(StackItem->GetHandle(), StackItem->GetStackCount());
	}
	else {
		UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
		FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();
		InventoryItem->InitItem(NewHandle, NewShopItem);
		InventoryMap.Add(NewHandle, InventoryItem);
		OnItemAdded.Broadcast(InventoryItem);
		UE_LOG(LogTemp, Warning, TEXT("Server Adding Shop Item: %s, With ID:%d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), NewHandle.GetHandleID());
		Client_ItemAdded(NewHandle, NewShopItem);
		InventoryItem->ApplyGASModifications(OwnerAbilitySystemComponent);
	}
}

void UInventoryComponent::ConsumeItem(UInventoryItem* Item)
{
	if (!GetOwner()->HasAuthority()) return;
	if (!Item) return;
	Item->ApplyConsumeEffect(OwnerAbilitySystemComponent);
	if (!Item->ReduceStackCount()) {
		RemoveItem(Item);
	}
	else {
		OnItemStackCountChanged.Broadcast(Item->GetHandle(), Item->GetStackCount());
		Client_ItemStackChanged(Item->GetHandle(), Item->GetStackCount());
	}
}

void UInventoryComponent::RemoveItem(UInventoryItem* Item)
{
	if (!GetOwner()->HasAuthority()) return;
	Item->RemoveGASModifications(OwnerAbilitySystemComponent);
	OnItemRemoved.Broadcast(Item->GetHandle());
	InventoryMap.Remove(Item->GetHandle());
	Client_ItemRemoved(Item->GetHandle());
}

void UInventoryComponent::Client_ItemRemoved_Implementation(FInventoryItemHandle ItemHandle)
{
	if (GetOwner()->HasAuthority()) return;
	UInventoryItem* Item = GetInventoryItemFromHandle(ItemHandle);
	if (!Item) return;
	OnItemRemoved.Broadcast(ItemHandle);
	InventoryMap.Remove(ItemHandle);
}

void UInventoryComponent::Client_ItemStackChanged_Implementation(FInventoryItemHandle Handle, int NewCount)
{
	if (GetOwner()->HasAuthority()) return;
	UInventoryItem* FoundItem = GetInventoryItemFromHandle(Handle);
	if (FoundItem) {
		FoundItem->SetStackCount(NewCount);
		OnItemStackCountChanged.Broadcast(Handle, NewCount); 
	}
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
	if (IsFullFor(ItemToPurchase)) return;
	OwnerAbilitySystemComponent->ApplyModToAttribute(ULOLHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());
	GrantItem(ItemToPurchase);
}

bool UInventoryComponent::Server_Purchase_Validate(const UPA_ShopItem* ItemToPurchase)
{
	return true;
}