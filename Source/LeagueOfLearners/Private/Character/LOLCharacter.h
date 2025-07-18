// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "LOLCharacter.generated.h"

UCLASS()
class ALOLCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALOLCharacter();
	void ServerSideInit();
	void ClientSideInit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

/******************/
/*Gameplay Ability*/
/*****************/
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	class ULOLAbilitySystemComponent* LOLAbilitySystemComponent;
	UPROPERTY()
	class ULOLAttributeSet* LOLAttributeSet;
};
