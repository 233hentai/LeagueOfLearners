// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/TA_Blackhole.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

ATA_Blackhole::ATA_Blackhole()
{
	RootComp = CreateDefaultSubobject<USceneComponent>("Root Component");
	SetRootComponent(RootComp);

	DetectionSphereComponent = CreateDefaultSubobject<USphereComponent>("Detection Sphere Component");
	DetectionSphereComponent->SetupAttachment(GetRootComponent());
	DetectionSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectionSphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	DetectionSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ATA_Blackhole::ActorEnterBlackholeRange);
	DetectionSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ATA_Blackhole::ActorLeftBlackholeRange);

	bReplicates = true;
	ShouldProduceTargetDataOnServer = true;
	PrimaryActorTick.bCanEverTick = true;

	VFXComponent = CreateDefaultSubobject<UParticleSystemComponent>("VFX Component");
	VFXComponent->SetupAttachment(GetRootComponent());
}

void ATA_Blackhole::ConfigureBlackhole(float InBlackholeRange, float InPullSpeed, float InBlackholeDuration, const FGenericTeamId& InTeamId)
{
	PullSpeed = InPullSpeed;
	BlackholeDuration = InBlackholeDuration;
	DetectionSphereComponent->SetSphereRadius(InBlackholeRange);
	BlackholeRange = InBlackholeRange;
	SetGenericTeamId(InTeamId);
}

void ATA_Blackhole::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamId = NewTeamID;
}

void ATA_Blackhole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATA_Blackhole, TeamId);
	DOREPLIFETIME_CONDITION_NOTIFY(ATA_Blackhole, BlackholeRange, COND_None, REPNOTIFY_Always);
}

void ATA_Blackhole::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	UWorld* World = GetWorld();
	if (World) {
		World->GetTimerManager().SetTimer(BlackholeDurationTimerHandle, this, &ATA_Blackhole::StopBlackhole, BlackholeDuration);
	}
}

void ATA_Blackhole::Tick(float DeltaTime)
{
	if (HasAuthority()) {
		for (TPair<AActor*, UNiagaraComponent*>& TargetPair : ActorsInRangeMap) {
			AActor* Target = TargetPair.Key;
			UNiagaraComponent* NiagaraComponent = TargetPair.Value;
			FVector PullDirection = (GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
			Target->SetActorLocation(Target->GetActorLocation() + PullDirection * PullSpeed * DeltaTime);
			if (NiagaraComponent) {
				NiagaraComponent->SetVariablePosition(BlackholeVFXOriginVariableName, VFXComponent->GetComponentLocation());
			}
		}
	}
}

void ATA_Blackhole::ConfirmTargetingAndContinue()
{
	StopBlackhole();
}

void ATA_Blackhole::CancelTargeting()
{
	StopBlackhole();
	Super::CancelTargeting();
}

void ATA_Blackhole::ActorEnterBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TryAddATarget(OtherActor);
}

void ATA_Blackhole::ActorLeftBlackholeRange(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	TryRemoveATarget(OtherActor);
}

void ATA_Blackhole::OnRep_BlackholeRange()
{
	DetectionSphereComponent->SetSphereRadius(BlackholeRange);
}

void ATA_Blackhole::TryAddATarget(AActor* OtherTarget)
{
	if (!OtherTarget || ActorsInRangeMap.Contains(OtherTarget)) return;
	if (GetTeamAttitudeTowards(*OtherTarget) != ETeamAttitude::Hostile) return;
	UNiagaraComponent* NiagaraComponent = nullptr;
	if (BlackholeLinkVFX) {
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(BlackholeLinkVFX, OtherTarget->GetRootComponent(), NAME_None, FVector::Zero(), FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false);
		if (NiagaraComponent) {
			NiagaraComponent->SetVariablePosition(BlackholeVFXOriginVariableName, VFXComponent->GetComponentLocation());
		}
	}
	ActorsInRangeMap.Add(OtherTarget, NiagaraComponent);
}

void ATA_Blackhole::TryRemoveATarget(AActor* OtherTarget)
{
	if (!OtherTarget) return;
	if (ActorsInRangeMap.Contains(OtherTarget)) {
		UNiagaraComponent* VFXComp;
		ActorsInRangeMap.RemoveAndCopyValue(OtherTarget, VFXComp);
		if (IsValid(VFXComp)) {
			VFXComp->DestroyComponent();
		}
	}
}

void ATA_Blackhole::StopBlackhole()
{
	TArray<TWeakObjectPtr<AActor>> FinalTargets;
	for (TPair<AActor*, UNiagaraComponent*>& TargetPair : ActorsInRangeMap) {
		FinalTargets.Add(TargetPair.Key);
		UNiagaraComponent* NiagaraComponent = TargetPair.Value;
		if (IsValid(NiagaraComponent)) {
			NiagaraComponent->DestroyComponent();
		}
	}
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	FGameplayAbilityTargetData_ActorArray* TargetDataArray = new FGameplayAbilityTargetData_ActorArray;
	TargetDataArray->SetActors(FinalTargets);
	TargetDataHandle.Add(TargetDataArray);
	FGameplayAbilityTargetData_SingleTargetHit* BlastLocation = new FGameplayAbilityTargetData_SingleTargetHit;
	BlastLocation->HitResult.ImpactPoint = GetActorLocation();
	TargetDataHandle.Add(BlastLocation);
	TargetDataReadyDelegate.Broadcast(TargetDataHandle);
}
