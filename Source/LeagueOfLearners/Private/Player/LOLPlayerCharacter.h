// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LOLCharacter.h"
#include "InputActionValue.h"
#include "GAS/LOLGameplayAbilityTypes.h"
#include "LOLPlayerCharacter.generated.h"


/**
 * 
 */
UCLASS()
class ALOLPlayerCharacter : public ALOLCharacter
{
	GENERATED_BODY()
public:
	ALOLPlayerCharacter();
	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "View", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "View", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* ViewCamera;

	FVector GetLookRightDirection() const;
	FVector GetLookForwardDirection() const;
	FVector GetMoveForwardDirection() const;

/***************************************************************/
/*                           Input                             */
/***************************************************************/
private:
	UPROPERTY(EditDefaultsOnly, Category = "INPUT")
	class UInputAction* JumpInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "INPUT")
	class UInputAction* LookInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "INPUT")
	class UInputAction* MoveInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "INPUT")
	TMap<ELOLAbilityInputID, class UInputAction*> GameplayAbilityInputActions;

	UPROPERTY(EditDefaultsOnly, Category = "INPUT")
	class UInputMappingContext* GameplayInputMappingContext;
	
	void HandleLookInput(const FInputActionValue& InputActionValue);
	void HandleMoveInput(const FInputActionValue& InputActionValue);

	void HandleAbilityInput(const FInputActionValue& InputActionValue,ELOLAbilityInputID InputID);
	void SetInputEnabledFromPlayerController(bool bEnabled);

/*******************************************************************/
/*                       Death and Respawn                         */
/*******************************************************************/
private:
	virtual void OnDead() override;
	virtual void OnRespawn() override;

/*******************************************************************/
/*                              Stun                               */
/*******************************************************************/
private:
	virtual void OnStun() override;
	virtual void OnRecoverFromStun() override;

/*******************************************************************/
/*                        Gameplay Ability                         */
/*******************************************************************/
private:
	virtual void OnAimStateChanged(bool bIsAiming) override;

/*******************************************************************/
/*                          Camera View                            */
/*******************************************************************/
private:
	UPROPERTY(EditDefaultsOnly, Category = "View")
	FVector CameraAimLocalOffset;
	UPROPERTY(EditDefaultsOnly, Category = "View")
	float CameraLerpSpeed = 20.f;
	FTimerHandle CameraLerpTimerHandle;

	void LerpCameraToLocalOffsetLocation(const FVector& Goal);
	void TickCameraLocalOffsetLerp(FVector Goal);
};
