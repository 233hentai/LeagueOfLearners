// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Projectile.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>("Root Component");
	SetRootComponent(RootComp);
	bReplicates = true;
}

void AProjectile::ShootProjectile(float InSpeed, float InMaxDistance, const AActor* InTarget, FGenericTeamId InTeamId, FGameplayEffectSpecHandle InHitEffectHandle)
{
	Target = InTarget;
	Speed = InSpeed;
	FRotator OwnerViewRotation = GetActorRotation();
	SetGenericTeamId(InTeamId);
	if (GetOwner()) {
		FVector OwnerViewLocation;
		GetOwner()->GetActorEyesViewPoint(OwnerViewLocation, OwnerViewRotation);
	}
	MoveDirection = OwnerViewRotation.Vector();
	HitEffectSpecHandle = InHitEffectHandle;
	float MaxFlyTime = InMaxDistance / InSpeed;
	GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &AProjectile::MaxDistanceReached, MaxFlyTime);
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectile, MoveDirection);
	DOREPLIFETIME(AProjectile, TeamId);
	DOREPLIFETIME(AProjectile, Speed);

}

void AProjectile::MaxDistanceReached()
{
	Destroy();
}

void AProjectile::SendLocalGameplayCue(AActor* CueTargetActor, const FHitResult& HitResult)
{
	FGameplayCueParameters CueParameters;
	CueParameters.Location = HitResult.ImpactPoint;
	CueParameters.Normal = HitResult.ImpactNormal;

	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(CueTargetActor, HitGameplayCueTag, EGameplayCueEvent::Executed, CueParameters);
}

void AProjectile::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

void AProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!OtherActor || OtherActor == GetOwner()) return;
	if (GetTeamAttitudeTowards(*OtherActor) != ETeamAttitude::Hostile) return;

	UAbilitySystemComponent* OtherASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (IsValid(OtherASC)) {
		if (HasAuthority() && HitEffectSpecHandle.IsValid()) {
			OtherASC->ApplyGameplayEffectSpecToSelf(*HitEffectSpecHandle.Data.Get());
			GetWorldTimerManager().ClearTimer(ShootTimerHandle);
		}
		FHitResult HitResult;
		HitResult.ImpactPoint = GetActorLocation();
		HitResult.ImpactNormal = GetActorForwardVector();
		SendLocalGameplayCue(OtherActor, HitResult);
		Destroy();
	}
}


// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasAuthority()) {
		if (Target) {
			MoveDirection = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		}
	}
	SetActorLocation(GetActorLocation() + MoveDirection * Speed * DeltaTime);
}

