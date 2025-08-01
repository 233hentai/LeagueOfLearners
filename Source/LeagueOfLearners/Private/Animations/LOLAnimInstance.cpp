// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/LOLAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/LOLAbilitySystemStatics.h"

void ULOLAnimInstance::NativeInitializeAnimation()
{
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter) {
		OwnerMovementComponent = OwnerCharacter->GetCharacterMovement();
	}

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner());
	if (OwnerASC) {
		OwnerASC->RegisterGameplayTagEvent(ULOLAbilitySystemStatics::GetAimStatTag()).AddUObject(this,&ULOLAnimInstance::OwnerAimTagChanged);
	}
}

void ULOLAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (OwnerCharacter) {
		FVector Velocity = OwnerCharacter->GetVelocity();
		Speed = Velocity.Length();
		FRotator BodyRotation = OwnerCharacter->GetActorRotation();
		FRotator BodyRotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(BodyRotation,BodyRotationPrevious);
		BodyRotationPrevious = BodyRotation;
		YawSpeed = BodyRotationDelta.Yaw / DeltaSeconds;
		SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(SmoothedYawSpeed,YawSpeed,DeltaSeconds,YawSpeedSmoothLerpSpeed);

		FRotator ControlRotation = OwnerCharacter->GetBaseAimRotation();
		LookRotationOffset = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation,BodyRotation);

		ForwardSpeed = Velocity.Dot(ControlRotation.Vector());
		RightSpeed= -Velocity.Dot(ControlRotation.Vector().Cross(FVector::UpVector));
	}
	if (OwnerMovementComponent) {
		bIsJumping = OwnerMovementComponent->IsFalling();
	}
}

void ULOLAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
}

bool ULOLAnimInstance::ShouldDoFullBody() const
{
	return (GetSpeed() <= 0) && !(IsAiming());
}

void ULOLAnimInstance::OwnerAimTagChanged(FGameplayTag Tag, int32 NewCount)
{
	bIsAiming = NewCount != 0;
}
