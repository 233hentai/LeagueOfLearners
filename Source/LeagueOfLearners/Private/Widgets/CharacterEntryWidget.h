// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CharacterEntryWidget.generated.h"

class UPA_HeroDefinition;
/**
 * 
 */
UCLASS()
class UCharacterEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	FORCEINLINE const UPA_HeroDefinition* GetHeroDefinition() const { return HeroDefinition; }
	void SetSelected(bool bIsSelected);

private:
	UPROPERTY(meta = (BindWidget))
	class UImage* CharacterIcon;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CharacterNameText;
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName IconTextureParamName = "Icon";
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FName SaturationParamName = "Saturation";

	UPROPERTY()
	const UPA_HeroDefinition* HeroDefinition;

};
