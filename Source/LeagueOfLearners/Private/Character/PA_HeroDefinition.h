// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GAS/LOLGameplayAbilityTypes.h"
#include "PA_HeroDefinition.generated.h"

class ALOLCharacter;
class UGameplayAbility;
/**
 * 
 */
UCLASS()
class UPA_HeroDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	static FPrimaryAssetType GetHeroDefinitionAssetType();

	FString GetHeroDisplayName() const { return HeroName; }
	UTexture2D* LoadIcon() const;
	TSubclassOf<ALOLCharacter> LoadCharacterClass() const;
	TSubclassOf<UAnimInstance> LoadDisplayAnimationBP() const;
	class USkeletalMesh* LoadDisplayMesh() const;
	const TMap<ELOLAbilityInputID, TSubclassOf<UGameplayAbility>>* GetAbilities() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	FString HeroName;
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftObjectPtr<UTexture2D> HeroIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftClassPtr<ALOLCharacter> CharacterClass;
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSoftClassPtr<UAnimInstance> DisplayAnimationBP;
};
