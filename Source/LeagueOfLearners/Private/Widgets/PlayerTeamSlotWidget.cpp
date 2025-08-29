// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PlayerTeamSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Character/PA_HeroDefinition.h"

void UPlayerTeamSlotWidget::UpdateSlot(const FString& PlayerName, const UPA_HeroDefinition* HeroDefinition)
{
	CachedPlayerName = PlayerName;
	if (HeroDefinition) {
		PlayerCharacterIcon->GetDynamicMaterial()->SetTextureParameterValue(CharacterIconMatParamName, HeroDefinition->LoadIcon());
		PlayerCharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(CharacterEmptyMatParamName, 0);
		CachedCharacterName = HeroDefinition->GetHeroDisplayName();
	}
	else {
		PlayerCharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(CharacterEmptyMatParamName, 1);
		CachedCharacterName = "";
	}
	UpdateNameText();
}

void UPlayerTeamSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerCharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(CharacterEmptyMatParamName, 1);
	CachedCharacterName = "";
}

void UPlayerTeamSlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	NameText->SetText(FText::FromString(CachedCharacterName));
	PlayAnimationForward(HoverAnim);
}

void UPlayerTeamSlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	NameText->SetText(FText::FromString(CachedPlayerName));
	PlayAnimationReverse(HoverAnim);
}

void UPlayerTeamSlotWidget::UpdateNameText()
{
	if (IsHovered()) {
		NameText->SetText(FText::FromString(CachedCharacterName));
	}
	else {
		NameText->SetText(FText::FromString(CachedPlayerName));
	}
}
