// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PA_CharacterDefinition.h"
#include "Character/LOLCharacter.h"

FPrimaryAssetId UPA_CharacterDefinition::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(GetCharacterDefinitionAssetType(),GetFName());
}

FPrimaryAssetType UPA_CharacterDefinition::GetCharacterDefinitionAssetType()
{
	return FPrimaryAssetType("CharacterDefinition");
}

UTexture2D* UPA_CharacterDefinition::LoadIcon() const
{
	CharacterIcon.LoadSynchronous();
	if (CharacterIcon.IsValid()) {
		return CharacterIcon.Get();
	}
	return nullptr;
}

TSubclassOf<ALOLCharacter> UPA_CharacterDefinition::LoadCharacterClass() const
{
	CharacterClass.LoadSynchronous();
	if (CharacterClass.IsValid()) {
		return CharacterClass.Get();
	}
	return TSubclassOf<ALOLCharacter>();
}

TSubclassOf<UAnimInstance> UPA_CharacterDefinition::LoadDisplayAnimationBP() const
{
	DisplayAnimationBP.LoadSynchronous();
	if (DisplayAnimationBP.IsValid()) {
		return DisplayAnimationBP.Get();
	}
	return TSubclassOf<UAnimInstance>();
}

USkeletalMesh* UPA_CharacterDefinition::LoadDisplayMesh() const
{
	TSubclassOf<ALOLCharacter> LoadedCharacter = LoadCharacterClass();
	if (!LoadedCharacter) return nullptr;

	ACharacter* Character = Cast<ACharacter>(LoadedCharacter.GetDefaultObject());
	if (!Character) return nullptr;

	return Character->GetMesh()->GetSkeletalMeshAsset();
}