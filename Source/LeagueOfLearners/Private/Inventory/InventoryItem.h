// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpecHandle.h"
#include "InventoryItem.generated.h"

class UPA_ShopItem;
class UAbilitySystemComponent;

USTRUCT()
struct FInventoryItemHandle
{
	GENERATED_BODY()
public:
	FInventoryItemHandle();
	static FInventoryItemHandle InvalidHandle();
	static FInventoryItemHandle CreateHandle();

	bool IsValid() const;
	uint32 GetHandleID() const { return HandleID; }

private:
	explicit FInventoryItemHandle(uint32 ID);

	UPROPERTY()
	uint32 HandleID;

	static uint32 GenerateNextID();
	static uint32 GetInvalidID();
};

bool operator==(const FInventoryItemHandle& Left, const FInventoryItemHandle& Right);
uint32 GetTypeHash(const FInventoryItemHandle& Key);
/**
 * 
 */
UCLASS()
class UInventoryItem : public UObject
{
	GENERATED_BODY()
	
public:
	UInventoryItem();
	void InitItem(const FInventoryItemHandle& NewHandle, const UPA_ShopItem* NewShopItem);
	const UPA_ShopItem* GetShopItem() const { return ShopItem; }
	FInventoryItemHandle GetHandle() const { return Handle; }
	void ApplyGASModifications(UAbilitySystemComponent* AbilitySystemComponent);
	void RemoveGASModifications(UAbilitySystemComponent* AbilitySystemComponent);
	bool IsValid() const;
	FORCEINLINE int GetStackCount() const { return StackCount; }
	void SetSlot(int NewSlot);
	bool IsStackFull() const;
	bool IsForItem(const UPA_ShopItem* Item) const;
	bool AddStackCount();
	bool ReduceStackCount();
	bool SetStackCount(int NewCount);
	bool TryActivateGrantedAbility(UAbilitySystemComponent* AbilitySystemComponent);
	void ApplyConsumeEffect(UAbilitySystemComponent* AbilitySystemComponent);

private:
	UPROPERTY()
	const UPA_ShopItem* ShopItem;
	FInventoryItemHandle Handle;
	int StackCount;
	int Slot;

	FActiveGameplayEffectHandle AppliedEquipedEffectHandle;
	FGameplayAbilitySpecHandle GrantedAbilitySpecHandle;
};
