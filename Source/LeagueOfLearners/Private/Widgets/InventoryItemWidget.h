// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/ItemWidget.h"
#include "Inventory/InventoryItem.h"
#include "InventoryItemWidget.generated.h"

class UInventoryItem;
class UInventoryItemWidget;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemDroppedDelegate, UInventoryItemWidget*/*Destination Widget*/, UInventoryItemWidget*/*Source Widget*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnButtonClickDelegate, const FInventoryItemHandle&);
/**
 * 
 */
UCLASS()
class UInventoryItemWidget : public UItemWidget
{
	GENERATED_BODY()

public:
	FOnInventoryItemDroppedDelegate OnInventoryItemDropped;
	FOnButtonClickDelegate OnLeftButtonClick;
	FOnButtonClickDelegate OnRightButtonClick;
	virtual void NativeConstruct() override;
	void UpdateInventoryItem(const UInventoryItem* Item);
	bool IsEmpty() const;
	void SetSlotNumber(int NewSlotNumber);
	FORCEINLINE int GetSlotNumber() const { return SlotNumber; }
	void EmptySlot();
	void UpdateStackCount();
	UTexture2D* GetIconTexture() const;
	FORCEINLINE const UInventoryItem* GetInventoryItem() const { return InventoryItem; }
	FInventoryItemHandle GetItemHandle() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	UTexture2D* EmptyTexture;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StackCountText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CooldownCountText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CooldownDurationText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ManaCostText;
	UPROPERTY()
	const UInventoryItem* InventoryItem;
	int SlotNumber;

	virtual void RightMouseButtonClicked() override;
	virtual void LeftMouseButtonClicked() override;

	void UpdateCanCastDisplay(bool bCanCast);

	/****************************************************/
	/*                      Drag Drop                   */
	/****************************************************/
private:
	UPROPERTY(EditDefaultsOnly, Category = "Drag Drop")
	TSubclassOf<class UInventoryItemDragDropOperation> DragDropOpClass;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

/****************************************************/
/*                        GAS                       */
/****************************************************/
public:
	void StartCooldown(float CooldownDuration, float TimeRemaining);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownUpdateInterval = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FName CooldownAmountDynamicMaterialParamName = "Percent";
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FName IconTextureDynamicMaterialParamName = "Icon";
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FName CanCastDynamicMaterialParamName = "CanCast";

	const UAbilitySystemComponent* OwnerAbilitySystemComponent;
	FTimerHandle CooldownDurationTimerHandle;
	FTimerHandle CooldownUpdateTimerHandle;
	float CooldownTimeRemaining = 0.f;
	float CooldownTimeDuration = 0.f;
	FNumberFormattingOptions CooldownDisplayFormattingOptions;

	void CooldownFinished();
	void UpdateCooldown();
	void ClearCooldown();
	virtual void SetIcon(UTexture2D* IconTexture) override;

	void BindCanCastAbilityDelegate();
	void UnbindCanCastAbilityDelegate();

};
