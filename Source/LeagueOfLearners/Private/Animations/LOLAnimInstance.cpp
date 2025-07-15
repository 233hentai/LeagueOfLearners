// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/LOLAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void ULOLAnimInstance::NativeInitializeAnimation()
{
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter) {
		OwnerMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void ULOLAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (OwnerCharacter) {
		Speed = OwnerCharacter->GetVelocity().Length();
		FRotator BodyRotation = OwnerCharacter->GetActorRotation();
		FRotator BodyRotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(BodyRotation,BodyRotationPrevious);
		BodyRotationPrevious = BodyRotation;
		YawSpeed = BodyRotationDelta.Yaw / DeltaSeconds;
		SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(SmoothedYawSpeed,YawSpeed,DeltaSeconds,YawSpeedSmoothLerpSpeed);
	}
}

void ULOLAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
}
