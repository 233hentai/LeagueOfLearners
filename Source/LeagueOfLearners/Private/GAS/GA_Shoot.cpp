// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GA_Shoot.h"
#include "GAS/LOLAbilitySystemStatics.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GAS/Projectile.h"
#include "GameplayTagsManager.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

UGA_Shoot::UGA_Shoot()
{
	ActivationOwnedTags.AddTag(ULOLAbilitySystemStatics::GetAimStatTag());
	ActivationOwnedTags.AddTag(ULOLAbilitySystemStatics::GetCrosshairTag());
}

void UGA_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!K2_CommitAbility()) {
		K2_EndAbility();
		return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Shoot Ability Activated"));
	if (HasAuthorityOrPredictionKey(ActorInfo,&ActivationInfo)) {
		UAbilityTask_WaitGameplayEvent* WaitStartShootEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ULOLAbilitySystemStatics::GetBasicAttackInputPressedTag());
		WaitStartShootEvent->EventReceived.AddDynamic(this, &UGA_Shoot::StartShooting);
		WaitStartShootEvent->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitStopShootEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ULOLAbilitySystemStatics::GetBasicAttackInputReleasedTag());
		WaitStopShootEvent->EventReceived.AddDynamic(this, &UGA_Shoot::StopShooting);
		WaitStopShootEvent->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitShootProjectileEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetShootTag(),nullptr,false,false);
		WaitShootProjectileEvent->EventReceived.AddDynamic(this, &UGA_Shoot::ShootProjectile);
		WaitShootProjectileEvent->ReadyForActivation();
	}
}

void UGA_Shoot::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	//UE_LOG(LogTemp, Warning, TEXT("Shoot ability ended"));
	K2_EndAbility();
}

void UGA_Shoot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (AimTargetASC) {
		AimTargetASC->RegisterGameplayTagEvent(ULOLAbilitySystemStatics::GetDeadStatTag()).RemoveAll(this);
		AimTargetASC = nullptr;
	}
	SendLocalGameplayEvent(ULOLAbilitySystemStatics::GetTargetUpdatedTag(), FGameplayEventData());
	StopShooting(FGameplayEventData());
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FGameplayTag UGA_Shoot::GetShootTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.Shoot");
}

void UGA_Shoot::StartShooting(FGameplayEventData EventData)
{
	//UE_LOG(LogTemp, Warning, TEXT("Start Shooting"));
	if (HasAuthority(&CurrentActivationInfo)) {
		UAbilityTask_PlayMontageAndWait* PlayShootMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ShootMontage);
		PlayShootMontage->ReadyForActivation();
	}
	else {
		PlayMontageLocally(ShootMontage);
	}
	FindAimTarget();
	StartAimTargetCheckTimer();
}

void UGA_Shoot::StopShooting(FGameplayEventData EventData)
{
	//UE_LOG(LogTemp, Warning, TEXT("Stop Shooting"));
	if (ShootMontage) {
		StopMontageAfterCurrentSection(ShootMontage);
	}
	StopAimTargetCheckTimer();
}

void UGA_Shoot::ShootProjectile(FGameplayEventData EventData)
{
	//UE_LOG(LogTemp, Warning, TEXT("Shoot Projectile"));
	if (K2_HasAuthority()) {
		AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = OwnerAvatarActor;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FVector SocketLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
		USkeletalMeshComponent* MeshComponent = GetOwningComponentFromActorInfo();
		if (MeshComponent) {
			TArray<FName> OutNames;
			UGameplayTagsManager::Get().SplitGameplayTagFName(EventData.EventTag, OutNames);
			if (OutNames.Num() != 0) {
				FName SocketName = OutNames.Last();
				SocketLocation = MeshComponent->GetSocketLocation(SocketName);
			}
		}
		AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SocketLocation, OwnerAvatarActor->GetActorRotation(), SpawnParameters);
		if (Projectile) {
			Projectile->ShootProjectile(ShootProjectileSpeed, ShootProjectileRange, GetAimTargetIfValid(), GetOwnerTeamId(), MakeOutgoingGameplayEffectSpec(ProjectileHitEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo)));
		}
	}
}

AActor* UGA_Shoot::GetAimTargetIfValid() const
{
	if (HasValidTarget()) return AimTarget;
	return nullptr;
}

void UGA_Shoot::FindAimTarget()
{
	if (HasValidTarget()) return;
	if (AimTargetASC) {
		AimTargetASC->RegisterGameplayTagEvent(ULOLAbilitySystemStatics::GetDeadStatTag()).RemoveAll(this);
		AimTargetASC = nullptr;
	}
	AimTarget = GetAimTarget(ShootProjectileRange, ETeamAttitude::Hostile);
	if (AimTarget) {
		AimTargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AimTarget);
		if (AimTargetASC) {
			AimTargetASC->RegisterGameplayTagEvent(ULOLAbilitySystemStatics::GetDeadStatTag()).AddUObject(this, &UGA_Shoot::TargetDeadTagUpdated);
		}
	}
	FGameplayEventData EventData;
	EventData.Target = AimTarget;
	SendLocalGameplayEvent(ULOLAbilitySystemStatics::GetTargetUpdatedTag(), EventData);
}

void UGA_Shoot::StartAimTargetCheckTimer()
{
	UWorld* World = GetWorld();
	if (World) {
		World->GetTimerManager().SetTimer(AimTargetCheckTimerHandle, this, &UGA_Shoot::FindAimTarget, AimTargetCheckInterval, true);
	}
}

void UGA_Shoot::StopAimTargetCheckTimer()
{
	UWorld* World = GetWorld();
	if (World) {
		World->GetTimerManager().ClearTimer(AimTargetCheckTimerHandle);
	}
}

bool UGA_Shoot::HasValidTarget() const
{
	if (!AimTarget) return false;
	if (ULOLAbilitySystemStatics::IsActorDead(AimTarget)) return false;
	if (!IsTargetInRange()) return false;
	return true;
}

bool UGA_Shoot::IsTargetInRange() const
{
	if (!AimTarget) return false;
	float Distance = FVector::Distance(AimTarget->GetActorLocation(), GetAvatarActorFromActorInfo()->GetActorLocation());
	return Distance <= ShootProjectileRange;
}

void UGA_Shoot::TargetDeadTagUpdated(const FGameplayTag Tag, int NewCount)
{
	if (NewCount > 0) {
		FindAimTarget();
	}
}
