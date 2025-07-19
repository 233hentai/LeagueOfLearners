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
	bool IsControlledByLocalPlayer() const;
	//���ڷ������˵���
	virtual void PossessedBy(AController* NewController) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

/*******************************************************************/
/*                       Gameplay Ability                          */
/*******************************************************************/
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "UI")
	class ULOLAbilitySystemComponent* LOLAbilitySystemComponent;
	UPROPERTY()
	class ULOLAttributeSet* LOLAttributeSet;

/*******************************************************************/
/*                              UI                                 */
/*******************************************************************/
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	class UWidgetComponent* OverHeadWidgetComponent;
	void ConfigureOverHeadStatsWidget();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HeadStatsGaugeVisibilityCheckGap = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HeadStatsGaugeVisibilityRangeSquared = 1000000.f;//�����ƽ�������Ǿ��룬��һ�ο�������
	FTimerHandle OverHeadStatsGaugeVisibilityHandle;
	void UpdateHeadStatsGaugeVisibility();
};
