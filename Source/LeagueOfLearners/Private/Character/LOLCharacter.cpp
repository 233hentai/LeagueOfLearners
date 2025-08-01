// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LOLCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/LOLAbilitySystemComponent.h"
#include "GAS/LOLAttributeSet.h"
#include "GAS/LOLAbilitySystemStatics.h"
#include "Widgets/OverHeadStatsGauge.h"
#include "Kismet/GameplayStatics.h"
#include "Net/Unrealnetwork.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "AbilitySystemBlueprintLibrary.h"

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

	BindGASChangeDelegates();

	PerceptionStimuliSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("Perception Stimuli Source Component");
}

void ALOLCharacter::ServerSideInit()
{
	LOLAbilitySystemComponent->InitAbilityActorInfo(this,this);
	LOLAbilitySystemComponent->ApplyInitialEffects();
	LOLAbilitySystemComponent->GiveInitialAbilities();
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

void ALOLCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALOLCharacter,TeamID);
}

const TMap<ELOLAbilityInputID, TSubclassOf<UGameplayAbility>>& ALOLCharacter::GetAbilities() const 
{
	return LOLAbilitySystemComponent->GetAbilities();
}

// Called when the game starts or when spawned
void ALOLCharacter::BeginPlay()
{
	Super::BeginPlay();
	ConfigureOverHeadStatsWidget();
	MeshRelativeTransform = GetMesh()->GetRelativeTransform();

	PerceptionStimuliSourceComponent->RegisterForSense(UAISense_Sight::StaticClass());
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

bool ALOLCharacter::Server_SendGameplayEventsToSelf_Validate(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
	return true;
}

void ALOLCharacter::Server_SendGameplayEventsToSelf_Implementation(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, EventData);
}


void ALOLCharacter::BindGASChangeDelegates()
{
	if (LOLAbilitySystemComponent) {
		LOLAbilitySystemComponent->RegisterGameplayTagEvent(ULOLAbilitySystemStatics::GetDeadStatTag()).AddUObject(this,&ALOLCharacter::DeathTagUpdated);
		LOLAbilitySystemComponent->RegisterGameplayTagEvent(ULOLAbilitySystemStatics::GetStunStatTag()).AddUObject(this, &ALOLCharacter::StunTagUpdated);
		LOLAbilitySystemComponent->RegisterGameplayTagEvent(ULOLAbilitySystemStatics::GetAimStatTag()).AddUObject(this, &ALOLCharacter::AimTagUpdated);
	}
}

void ALOLCharacter::DeathTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount != 0) {
		StartDeathSequence();
	}
	else
	{
		Respawn();
	}
}

void ALOLCharacter::StunTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (IsDead()) return;
	if (NewCount != 0) {
		OnStun();
		PlayAnimMontage(StunMontage);
	}
	else {
		OnRecoverFromStun();
		StopAnimMontage(StunMontage);
	}
}

void ALOLCharacter::AimTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	SetIsAiming(NewCount!=0);
}

void ALOLCharacter::SetIsAiming(bool bIsAiming)
{
	bUseControllerRotationYaw = bIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !bIsAiming;
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

void ALOLCharacter::SetStatusGaugeEnabled(bool bIsEnabled)
{
	GetWorldTimerManager().ClearTimer(OverHeadStatsGaugeVisibilityHandle);
	if (bIsEnabled) {
		ConfigureOverHeadStatsWidget();
	}
	else {
		OverHeadWidgetComponent->SetHiddenInGame(true);
	}
}

bool ALOLCharacter::IsDead() const
{
	return GetAbilitySystemComponent()->HasMatchingGameplayTag(ULOLAbilitySystemStatics::GetDeadStatTag());
}

void ALOLCharacter::RespawnImmediately()
{
	if (HasAuthority()) {
		GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(ULOLAbilitySystemStatics::GetDeadStatTag()));
	}
}

void ALOLCharacter::PlayDeathMontage()
{
	if (DeathMontage) {
		float MontageTime=PlayAnimMontage(DeathMontage);	
		GetWorldTimerManager().SetTimer(DeathMontageTimerHandle,this, &ALOLCharacter::DeathMontageFinished, MontageTime+DeathMontageFinishTimeShift);
	}
}

void ALOLCharacter::StartDeathSequence()
{
	if (LOLAbilitySystemComponent) {
		LOLAbilitySystemComponent->CancelAbilities();
	}

	OnDead();
	PlayDeathMontage();
	SetStatusGaugeEnabled(false);
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetAIPerceptionStimuliSourceEnabled(false);
}

void ALOLCharacter::Respawn()
{
	OnRespawn();
	SetRagDollEnabled(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
	SetStatusGaugeEnabled(true);
	SetAIPerceptionStimuliSourceEnabled(true);

	if (HasAuthority()) {
		TWeakObjectPtr<AActor> StartSpot = GetController()->StartSpot;
		if (StartSpot.IsValid()) {
			SetActorTransform(StartSpot->GetActorTransform());
		}
	}

	if (LOLAbilitySystemComponent) {
		LOLAbilitySystemComponent->ApplyFullStatEffect();
	}
}

void ALOLCharacter::OnDead()
{
}

void ALOLCharacter::OnRespawn()
{
}

void ALOLCharacter::DeathMontageFinished()
{
	if (IsDead()) {
		SetRagDollEnabled(true);
	}
}

void ALOLCharacter::SetRagDollEnabled(bool bIsEnabled)
{
	if (bIsEnabled) {
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	else {
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AttachToComponent(GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
		GetMesh()->SetRelativeTransform(MeshRelativeTransform);
	}
}

void ALOLCharacter::OnStun()
{
}

void ALOLCharacter::OnRecoverFromStun()
{
}

void ALOLCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId ALOLCharacter::GetGenericTeamId() const
{
	return TeamID;
}

void ALOLCharacter::OnRep_TeamID()
{
}

void ALOLCharacter::SetAIPerceptionStimuliSourceEnabled(bool bIsEnabled)
{
	if (!PerceptionStimuliSourceComponent) return;
	if (bIsEnabled) {
		PerceptionStimuliSourceComponent->RegisterWithPerceptionSystem();
	}
	else {
		PerceptionStimuliSourceComponent->UnregisterFromPerceptionSystem();
	}
}

