// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/LOLGameplayAbility.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GAS/GAP_Launched.h"
#include "GAS/LOLAbilitySystemStatics.h"

ULOLGameplayAbility::ULOLGameplayAbility()
{
    ActivationBlockedTags.AddTag(ULOLAbilitySystemStatics::GetStunStatTag());
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
