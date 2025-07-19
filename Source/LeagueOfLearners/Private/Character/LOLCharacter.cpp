// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LOLCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GAS/LOLAbilitySystemComponent.h"
#include "GAS/LOLAttributeSet.h"
#include "Widgets/OverHeadStatsGauge.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
ALOLCharacter::ALOLCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LOLAbilitySystemComponent = CreateDefaultSubobject<ULOLAbilitySystemComponent>("LOLAbility System Component");
	LOLAttributeSet = CreateDefaultSubobject<ULOLAttributeSet>("LOLAttribute Set");
	OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Over Head Widget Component");
	OverHeadWidgetComponent->SetupAttachment(GetRootComponent());
}

void ALOLCharacter::ServerSideInit()
{
	LOLAbilitySystemComponent->InitAbilityActorInfo(this,this);
	LOLAbilitySystemComponent->ApplyInitialEffects();
}

void ALOLCharacter::ClientSideInit()
{
	LOLAbilitySystemComponent->InitAbilityActorInfo(this,this);
}

bool ALOLCharacter::IsControlledByLocalPlayer() const
{
	return GetController()&&GetController()->IsLocalController();
}

void ALOLCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController && !NewController->IsPlayerController()) {
		ServerSideInit();
	}
}

// Called when the game starts or when spawned
void ALOLCharacter::BeginPlay()
{
	Super::BeginPlay();
	ConfigureOverHeadStatsWidget();
	
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

UAbilitySystemComponent* ALOLCharacter::GetAbilitySystemComponent() const
{
	return LOLAbilitySystemComponent;
}

void ALOLCharacter::ConfigureOverHeadStatsWidget()
{
	if (OverHeadWidgetComponent) {
		if (IsControlledByLocalPlayer()) {
			OverHeadWidgetComponent->SetHiddenInGame(true);
			return;
		}
		UOverHeadStatsGauge* OverHeadStatsGauge = Cast<UOverHeadStatsGauge>(OverHeadWidgetComponent->GetUserWidgetObject());
		if (OverHeadStatsGauge) {
			OverHeadStatsGauge->ConfigureWithASC(GetAbilitySystemComponent());
			OverHeadWidgetComponent->SetHiddenInGame(false);
			GetWorldTimerManager().ClearTimer(OverHeadStatsGaugeVisibilityHandle);
			GetWorldTimerManager().SetTimer(OverHeadStatsGaugeVisibilityHandle, this, &ALOLCharacter::UpdateHeadStatsGaugeVisibility, HeadStatsGaugeVisibilityCheckGap, true);

		}
	}
	
}

void ALOLCharacter::UpdateHeadStatsGaugeVisibility()
{
	APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this,0);
	if (LocalPlayerPawn) {
		float DistanceSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());
		OverHeadWidgetComponent->SetHiddenInGame(DistanceSquared>HeadStatsGaugeVisibilityRangeSquared);
	}
}

