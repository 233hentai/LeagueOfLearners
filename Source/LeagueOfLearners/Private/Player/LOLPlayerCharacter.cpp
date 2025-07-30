// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LOLPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/LOLAbilitySystemStatics.h"

ALOLPlayerCharacter::ALOLPlayerCharacter()
{
	CameraArm = CreateDefaultSubobject<USpringArmComponent>("CameraArm");
	CameraArm->SetupAttachment(GetRootComponent());
	CameraArm->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>("ViewCamera");
	ViewCamera->SetupAttachment(CameraArm,USpringArmComponent::SocketName);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 720, 0);
}

void ALOLPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	APlayerController* OwingPlayerController = GetController<APlayerController>();
	if (OwingPlayerController) {
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = OwingPlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (InputSubsystem) {
			InputSubsystem->RemoveMappingContext(GameplayInputMappingContext);
			InputSubsystem->AddMappingContext(GameplayInputMappingContext,0);
		}
	}
}

void ALOLPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent) {
		EnhancedInputComponent->BindAction(JumpInputAction,ETriggerEvent::Triggered,this,&ALOLPlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ALOLPlayerCharacter::HandleLookInput);
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ALOLPlayerCharacter::HandleMoveInput);

		for (const TPair<ELOLAbilityInputID, class UInputAction*> InputActionPair : GameplayAbilityInputActions) {
			EnhancedInputComponent->BindAction(InputActionPair.Value, ETriggerEvent::Triggered, this, &ALOLPlayerCharacter::HandleAbilityInput, InputActionPair.Key);
		}
	}
}


void ALOLPlayerCharacter::HandleLookInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputValue = InputActionValue.Get<FVector2D>();
	AddControllerPitchInput(-InputValue.Y);
	AddControllerYawInput(InputValue.X);
}

void ALOLPlayerCharacter::HandleMoveInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputValue = InputActionValue.Get<FVector2D>();
	InputValue.Normalize();
	AddMovementInput(GetMoveForwardDirection()*InputValue.Y+GetLookRightDirection()*InputValue.X);
}

void ALOLPlayerCharacter::HandleAbilityInput(const FInputActionValue& InputActionValue, ELOLAbilityInputID InputID)
{
	bool bPressed = InputActionValue.Get<bool>();
	if (bPressed) {
		GetAbilitySystemComponent()->AbilityLocalInputPressed((int32)InputID);
	}
	else {
		GetAbilitySystemComponent()->AbilityLocalInputReleased((int32)InputID);
	}

	if (InputID == ELOLAbilityInputID::BasicAttack) {
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this,ULOLAbilitySystemStatics::GetBasicAttackInputPressedTag(), FGameplayEventData());
		Server_SendGameplayEventsToSelf(ULOLAbilitySystemStatics::GetBasicAttackInputPressedTag(), FGameplayEventData());
	}
}

void ALOLPlayerCharacter::SetInputEnabledFromPlayerController(bool bEnabled)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (!PlayerController) return;
	if(bEnabled)
	{
		EnableInput(PlayerController);
	}
	else {
		DisableInput(PlayerController);
	}
}

void ALOLPlayerCharacter::OnDead()
{
	SetInputEnabledFromPlayerController(false);
}

void ALOLPlayerCharacter::OnRespawn()
{
	SetInputEnabledFromPlayerController(true);
}

void ALOLPlayerCharacter::OnStun()
{
	SetInputEnabledFromPlayerController(false);
}

void ALOLPlayerCharacter::OnRecoverFromStun()
{
	if (IsDead()) return;
	SetInputEnabledFromPlayerController(true);
}

FVector ALOLPlayerCharacter::GetLookRightDirection() const
{
	return ViewCamera->GetRightVector();
}

FVector ALOLPlayerCharacter::GetLookForwardDirection() const
{
	return ViewCamera->GetForwardVector();
}

FVector ALOLPlayerCharacter::GetMoveForwardDirection() const
{
	return FVector::CrossProduct(GetLookRightDirection(),FVector::UpVector);
}
