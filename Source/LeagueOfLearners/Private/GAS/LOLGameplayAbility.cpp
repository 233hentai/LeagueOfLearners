// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/LOLGameplayAbility.h"
#include "Components/SkeletalMeshComponent.h"

UAnimInstance* ULOLGameplayAbility::GetOwnerAnimInstance() const
{
    USkeletalMeshComponent* OwnerSkeletalMeshComponent = GetOwningComponentFromActorInfo();
    if (OwnerSkeletalMeshComponent) {
        return OwnerSkeletalMeshComponent->GetAnimInstance();
    }
    return nullptr;
}
