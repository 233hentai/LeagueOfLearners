// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/PA_HeroDefinition.h"
#include "Character/LOLCharacter.h"

FPrimaryAssetId UPA_HeroDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetHeroDefinitionAssetType(), GetFName());
}

FPrimaryAssetType UPA_HeroDefinition::GetHeroDefinitionAssetType()
{
	return FPrimaryAssetType("HeroDefinition");
}

UTexture2D* UPA_HeroDefinition::LoadIcon() const
{
	HeroIcon.LoadSynchronous();
	if (HeroIcon.IsValid()) {
		return HeroIcon.Get();
	}
	return nullptr;
}

TSubclassOf<ALOLCharacter> UPA_HeroDefinition::LoadCharacterClass() const
{
	CharacterClass.LoadSynchronous();
	if (CharacterClass.IsValid()) {
		return CharacterClass.Get();
	}
	return TSubclassOf<ALOLCharacter>();
}

TSubclassOf<UAnimInstance> UPA_HeroDefinition::LoadDisplayAnimationBP() const
{
	DisplayAnimationBP.LoadSynchronous();
	if (DisplayAnimationBP.IsValid()) {
		return DisplayAnimationBP.Get();
	}
	return TSubclassOf<UAnimInstance>();
}

USkeletalMesh* UPA_HeroDefinition::LoadDisplayMesh() const
{
	TSubclassOf<ALOLCharacter> LoadedCharacter = LoadCharacterClass();
	if (!LoadedCharacter) return nullptr;

	ACharacter* Character = Cast<ACharacter>(LoadedCharacter.GetDefaultObject());
	if (!Character) return nullptr;

	return Character->GetMesh()->GetSkeletalMeshAsset();
}

const TMap<ELOLAbilityInputID, TSubclassOf<UGameplayAbility>>* UPA_HeroDefinition::GetAbilities() const
{
	TSubclassOf<ALOLCharacter> LoadedCharacter = LoadCharacterClass();
	if (!LoadedCharacter) return nullptr;
	ALOLCharacter* Character = Cast<ALOLCharacter>(LoadedCharacter.GetDefaultObject());
	if (!Character) return nullptr;

	return &(Character->GetAbilities());
}