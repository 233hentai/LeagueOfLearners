// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/LOLGameplayAbility.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/GAP_Launched.h"
#include "GAS/LOLAbilitySystemStatics.h"

ULOLGameplayAbility::ULOLGameplayAbility()
{
    ActivationBlockedTags.AddTag(ULOLAbilitySystemStatics::GetStunStatTag());
}

bool ULOLGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    FGameplayAbilitySpec* AbilitySpec = ActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
    if (AbilitySpec && AbilitySpec->Level <= 0) {
        return false;
    }
    return Super::CanActivateAbility(Handle,ActorInfo,SourceTags,TargetTags,OptionalRelevantTags);
}

UAnimInstance* ULOLGameplayAbility::GetOwnerAnimInstance() const
{
    USkeletalMeshComponent* OwnerSkeletalMeshComponent = GetOwningComponentFromActorInfo();
    if (OwnerSkeletalMeshComponent) {
        return OwnerSkeletalMeshComponent->GetAnimInstance();
    }
    return nullptr;
}

TArray<FHitResult> ULOLGameplayAbility::GetHitResultFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SweepSphereRadius, ETeamAttitude::Type TargetTeam, bool bDrawDebug, bool bIgnoreSelf) const
{
    TArray<FHitResult> OutResults;
    TSet<AActor*> HitActors;

    IGenericTeamAgentInterface* OwnerTeamInterFace = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());

    for (const TSharedPtr<FGameplayAbilityTargetData> TargetData : TargetDataHandle.Data) {
        FVector StartLocation = TargetData->GetOrigin().GetTranslation();
        FVector EndLocation = TargetData->GetEndPoint();

        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectsTypes;
        ObjectsTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

        TArray<AActor*> ActorsToIgnore;
        if (bIgnoreSelf) {
            ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
        }

        EDrawDebugTrace::Type DrawDebugTrace = bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

        TArray<FHitResult> Results;
        UKismetSystemLibrary::SphereTraceMultiForObjects(this,StartLocation,EndLocation, SweepSphereRadius, ObjectsTypes,false, ActorsToIgnore, DrawDebugTrace,Results,false);

        for (const FHitResult& Result : Results) {
            if (HitActors.Contains(Result.GetActor())) {
                continue;
            }
            if (OwnerTeamInterFace) {
                ETeamAttitude::Type OtherActorTeamAttitude = OwnerTeamInterFace->GetTeamAttitudeTowards(*Result.GetActor());
                if (OtherActorTeamAttitude != TargetTeam) continue;
            }
            HitActors.Add(Result.GetActor());
            OutResults.Add(Result);
        }
    }
    
    return OutResults;
}

void ULOLGameplayAbility::PushSelf(const FVector& PushVelocity)
{
    ACharacter* OwningAvatarCharacter = GetOwningAvatarActor();
    if (OwningAvatarCharacter) {
        OwningAvatarCharacter->LaunchCharacter(PushVelocity,true,true);
    }
}

void ULOLGameplayAbility::PushTarget(AActor* Target, const FVector& PushVelocity)
{
    if (!Target) return;
    FGameplayEventData EventData;
    FGameplayAbilityTargetData_SingleTargetHit* HitData = new FGameplayAbilityTargetData_SingleTargetHit;
    FHitResult HitResult;
    HitResult.ImpactNormal = PushVelocity;
    HitData->HitResult = HitResult;
    EventData.TargetData.Add(HitData);
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target,UGAP_Launched::GetLaunchedAbilityActivationTag(), EventData);
}

void ULOLGameplayAbility::PushTargets(const TArray<AActor*>& Targets, const FVector& PushVelocity)
{
    for (AActor* Target : Targets) {
        PushTarget(Target, PushVelocity);
    }
}

void ULOLGameplayAbility::PushTargets(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& PushVelocity)
{
    TArray<AActor*> Targets = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetDataHandle);
    PushTargets(Targets,PushVelocity);
}

ACharacter* ULOLGameplayAbility::GetOwningAvatarActor()
{
    if (!AvatarCharacter) {
        AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
    }
    return AvatarCharacter;
}

void ULOLGameplayAbility::ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffect, int level)
{
    FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect, level);

    FGameplayEffectContextHandle EffectContext = MakeEffectContext(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo());
    EffectContext.AddHitResult(HitResult);
    EffectSpecHandle.Data->SetContext(EffectContext);

    ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(HitResult.GetActor()));
}

void ULOLGameplayAbility::PlayMontageLocally(UAnimMontage* MontageToPlay)
{
    UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
    if (OwnerAnimInstance&& !OwnerAnimInstance->Montage_IsPlaying(MontageToPlay)) {
        OwnerAnimInstance->Montage_Play(MontageToPlay);
    }
}

void ULOLGameplayAbility::StopMontageAfterCurrentSection(UAnimMontage* MontageToStop)
{
    UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
    if (OwnerAnimInstance) {
        FName CurrentSectionName = OwnerAnimInstance->Montage_GetCurrentSection(MontageToStop);
        OwnerAnimInstance->Montage_SetNextSection(CurrentSectionName, NAME_None, MontageToStop);
    }
}

FGenericTeamId ULOLGameplayAbility::GetOwnerTeamId() const
{
    IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
    if (OwnerTeamInterface) {
        return OwnerTeamInterface->GetGenericTeamId();
    }
    return FGenericTeamId::NoTeam;
}

AActor* ULOLGameplayAbility::GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const
{
    AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
    if (OwnerAvatarActor) {
        FVector Location;
        FRotator Rotation;
        OwnerAvatarActor->GetActorEyesViewPoint(Location, Rotation);
        FVector AimEnd = Location + Rotation.Vector() * AimDistance;
        TArray<FHitResult> HitResults;
        FCollisionQueryParams CollisionQueryParams;
        CollisionQueryParams.AddIgnoredActor(OwnerAvatarActor);
        FCollisionObjectQueryParams CollisionObjectQueryParams;
        CollisionObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
        if (ShouldDrawDebug()) {
            DrawDebugLine(GetWorld(), Location, AimEnd, FColor::Red, false, 2.f, 0U, 3.f);
        }
        if (GetWorld()->LineTraceMultiByObjectType(HitResults, Location, AimEnd, CollisionObjectQueryParams, CollisionQueryParams)) {
            for(FHitResult& HitResult:HitResults){
                if (IsActorTeamAttitudeIs(HitResult.GetActor(), TeamAttitude)) {
                    return HitResult.GetActor();
                }
            }
        }
    }
    return nullptr;
}

bool ULOLGameplayAbility::IsActorTeamAttitudeIs(const AActor* OtherActor, ETeamAttitude::Type TeamAttitude) const
{
    if (!OtherActor) return false;
    IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
    if (OwnerTeamInterface) {
        return OwnerTeamInterface->GetTeamAttitudeTowards(*OtherActor) == TeamAttitude;
    }
    return false;
}

void ULOLGameplayAbility::SendLocalGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
    UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
    if (OwnerASC) {
        OwnerASC->HandleGameplayEvent(EventTag, &EventData);
    }
}
