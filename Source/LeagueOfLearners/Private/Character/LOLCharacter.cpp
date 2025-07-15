// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LOLCharacter.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
ALOLCharacter::ALOLCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ALOLCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALOLCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALOLCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

