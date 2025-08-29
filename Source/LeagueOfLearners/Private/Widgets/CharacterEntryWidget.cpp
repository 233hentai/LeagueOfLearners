// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CharacterEntryWidget.h"
#include "Character/PA_HeroDefinition.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UCharacterEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
    IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

    HeroDefinition = Cast<UPA_HeroDefinition>(ListItemObject);
    if (HeroDefinition) {
        CharacterIcon->GetDynamicMaterial()->SetTextureParameterValue(IconTextureParamName, HeroDefinition->LoadIcon());
        CharacterNameText->SetText(FText::FromString(HeroDefinition->GetHeroDisplayName()));
    }
}


void UCharacterEntryWidget::SetSelected(bool bIsSelected)
{
	CharacterIcon->GetDynamicMaterial()->SetScalarParameterValue(SaturationParamName, bIsSelected ? 0.f : 1.f);
}