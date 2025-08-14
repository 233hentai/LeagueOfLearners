// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItem.h"
#include "AbilitySystemComponent.h"
#include "Inventory/PA_ShopItem.h"
#include "GameplayEffect.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/LOLAbilitySystemStatics.h"

FInventoryItemHandle::FInventoryItemHandle() :HandleID{GetInvalidID()}
{
}

FInventoryItemHandle FInventoryItemHandle::InvalidHandle()
{
	static FInventoryItemHandle InvalidHandle = FInventoryItemHandle();
	return InvalidHandle;
}

FInventoryItemHandle::FInventoryItemHandle(uint32 ID) :HandleID{ID}
{
}

FInventoryItemHandle FInventoryItemHandle::CreateHandle()
{
	return FInventoryItemHandle(GenerateNextID());
}

bool FInventoryItemHandle::IsValid() const
{
	return HandleID != GetInvalidID();
}

uint32 FInventoryItemHandle::GenerateNextID()
{
	static uint32 StaticID = 1;
	return StaticID++;
}

uint32 FInventoryItemHandle::GetInvalidID()
{
	return 0;
}

bool operator==(const FInventoryItemHandle& Left, const FInventoryItemHandle& Right)
{
	return Left.GetHandleID() == Right.GetHandleID();
}

uint32 GetTypeHash(const FInventoryItemHandle& Key)
{
	return Key.GetHandleID();
}

UInventoryItem::UInventoryItem() :StackCount{1}
{
}

void UInventoryItem::InitItem(const FInventoryItemHandle& NewHandle, const UPA_ShopItem* NewShopItem)
{
	Handle = NewHandle;
	ShopItem = NewShopItem;
}

void UInventoryItem::ApplyGASModifications(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!GetShopItem()) return;
	if (!AbilitySystemComponent->GetOwner() || !AbilitySystemComponent->GetOwner()->HasAuthority()) return;

	TSubclassOf<UGameplayEffect> EquipEffect = GetShopItem()->GetEquippedEffect();
	if (EquipEffect) {
		AppliedEquipedEffectHandle = AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(EquipEffect, 1, AbilitySystemComponent->MakeEffectContext());
	}
	TSubclassOf<UGameplayAbility> GrantedAbility = GetShopItem()->GetGrantedAbility();
	if (GrantedAbility) {
		const FGameplayAbilitySpec* FoundSpec = AbilitySystemComponent->FindAbilitySpecFromClass(GrantedAbility);
		if (FoundSpec) {
			GrantedAbilitySpecHandle = FoundSpec->Handle;
		}
		else {
			GrantedAbilitySpecHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(GrantedAbility));
		}
	}
}

void UInventoryItem::RemoveGASModifications(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!AbilitySystemComponent) return;
	if (AppliedEquipedEffectHandle.IsValid()) {
		AbilitySystemComponent->RemoveActiveGameplayEffect(AppliedEquipedEffectHandle);
	}
	if (GrantedAbilitySpecHandle.IsValid()) {
		AbilitySystemComponent->SetRemoveAbilityOnEnd(GrantedAbilitySpecHandle);
	}
}

bool UInventoryItem::IsValid() const
{
	return ShopItem != nullptr;
}

void UInventoryItem::SetSlot(int NewSlot)
{
	Slot = NewSlot;
}

bool UInventoryItem::IsStackFull() const
{
	return StackCount >= GetShopItem()->GetMaxStackCount();
}

bool UInventoryItem::IsForItem(const UPA_ShopItem* Item) const
{
	if (!Item) return false;
	return GetShopItem() == Item;
}

bool UInventoryItem::AddStackCount()
{
	if (IsStackFull()) return false;
	++StackCount;
	return true;
}

bool UInventoryItem::ReduceStackCount()
{
	--StackCount;
	if (StackCount <= 0) {
		return false;
	}
	return true;
}

bool UInventoryItem::SetStackCount(int NewCount)
{
	if (NewCount > 0 && NewCount <= GetShopItem()->GetMaxStackCount()) {
		StackCount = NewCount;
		return true;
	}
	return false;
}

bool UInventoryItem::TryActivateGrantedAbility(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!GrantedAbilitySpecHandle.IsValid()) return false;
	if (AbilitySystemComponent && AbilitySystemComponent->TryActivateAbility(GrantedAbilitySpecHandle)) return true;
	return false;
}

void UInventoryItem::ApplyConsumeEffect(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!ShopItem) return;
	TSubclassOf<UGameplayEffect> ConsumeEffect = ShopItem->GetConsumeEffect();
	if (!ConsumeEffect) return;
	AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(ConsumeEffect, 1, AbilitySystemComponent->MakeEffectContext());
}
