// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/LOLGameplayAbility.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

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
