// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "LOLAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class ULOLAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	ATTRIBUTE_ACCESSORS(ULOLAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(ULOLAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(ULOLAttributeSet, Mana);
	ATTRIBUTE_ACCESSORS(ULOLAttributeSet, MaxMana);
	ATTRIBUTE_ACCESSORS(ULOLAttributeSet, Attack);
	ATTRIBUTE_ACCESSORS(ULOLAttributeSet, Armor);
	ATTRIBUTE_ACCESSORS(ULOLAttributeSet, MoveSpeed);

	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
private:
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	UPROPERTY(ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	UPROPERTY(ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	UPROPERTY(ReplicatedUsing = OnRep_Attack)
	FGameplayAttributeData Attack;
	UPROPERTY(ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldData);
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldData);
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldData);
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldData);
	UFUNCTION()
	void OnRep_Attack(const FGameplayAttributeData& OldData);
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldData);
	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldData);
};
