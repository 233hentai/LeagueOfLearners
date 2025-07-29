// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/LOLAIController.h"
#include "Character/LOLCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/LOLAbilitySystemStatics.h"

ALOLAIController::ALOLAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception Component");
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Config");
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;
	SightConfig->SetMaxAge(5.f);
	SightConfig->PeripheralVisionAngleDegrees = 180.f;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this,&ALOLAIController::TargetPerceptionUpdated);
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this,&ALOLAIController::TargetForgotten);
}

void ALOLAIController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(NewPawn);
	if (PawnTeamInterface) {
		SetGenericTeamId(PawnTeamInterface->GetGenericTeamId());
		ClearAndDisableAllSenses();
		EnableAllSenses();
	}

	UAbilitySystemComponent* PawnASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(NewPawn);
	if (PawnASC) {
		PawnASC->RegisterGameplayTagEvent(ULOLAbilitySystemStatics::GetDeadStatTag()).AddUObject(this,&ALOLAIController::PawnDeadTagUpdated);
		PawnASC->RegisterGameplayTagEvent(ULOLAbilitySystemStatics::GetStunStatTag()).AddUObject(this, &ALOLAIController::PawnStunTagUpdated);
	}
}

void ALOLAIController::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(BehaviorTree);
}


void ALOLAIController::TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
	//UE_LOG(LogTemp, Warning, TEXT("Updated!"))
	if (Stimulus.WasSuccessfullySensed()) {
		if (!GetCurrentTarget()) {
			SetCurrentTarget(TargetActor);
		}
	}
	else {
		ForgetActorIfDead(TargetActor);
	}
}

void ALOLAIController::TargetForgotten(AActor* ForgottenActor)
{
	if (!ForgottenActor) return;
	if (GetCurrentTarget() == ForgottenActor) {
		SetCurrentTarget(GetNextPerceivedActor());
	}
}

const UObject* ALOLAIController::GetCurrentTarget() const
{
	const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (BlackboardComponent) {
		return GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKeyName);
	}
	return nullptr;
}

void ALOLAIController::SetCurrentTarget(AActor* NewTarget)
{
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (!BlackboardComponent) {
		//UE_LOG(LogTemp,Warning,TEXT("No BlackboardComponent!"))
		return;
	}
	if (NewTarget) {
		//UE_LOG(LogTemp, Warning, TEXT("Target set"))
		BlackboardComponent->SetValueAsObject(TargetBlackboardKeyName,NewTarget);
	}
	else {
		//UE_LOG(LogTemp, Warning, TEXT("Clear BlackboardComponent!"))
		BlackboardComponent->ClearValue(TargetBlackboardKeyName);
	}
}

AActor* ALOLAIController::GetNextPerceivedActor() const
{
	if (PerceptionComponent) {
		TArray<AActor*> Actors;
		AIPerceptionComponent->GetPerceivedHostileActors(Actors);
		if (Actors.Num() != 0) {
			return Actors[0];
		}
	}
	return nullptr;
}

void ALOLAIController::ForgetActorIfDead(AActor* ActorToForget)
{
	const UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToForget);
	if (!AbilitySystemComponent) return;
	if (AbilitySystemComponent->HasMatchingGameplayTag(ULOLAbilitySystemStatics::GetDeadStatTag())) {
		for (UAIPerceptionComponent::TActorPerceptionContainer::TIterator Iterator = AIPerceptionComponent->GetPerceptualDataIterator(); Iterator; ++Iterator) {
			if (Iterator->Key != ActorToForget) {
				continue;
			}
			for (FAIStimulus& Stimuli : Iterator->Value.LastSensedStimuli) {
				Stimuli.SetStimulusAge(TNumericLimits<float>::Max());
			}
		}
	}
}

void ALOLAIController::ClearAndDisableAllSenses()
{
	AIPerceptionComponent->AgeStimuli(TNumericLimits<float>::Max());
	for (auto SenseConfigIterator = AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIterator; ++SenseConfigIterator) {
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIterator)->GetSenseImplementation(),false);
	}
	if (GetBlackboardComponent()) {
		GetBlackboardComponent()->ClearValue(TargetBlackboardKeyName);
	}
}

void ALOLAIController::EnableAllSenses()
{
	for (auto SenseConfigIterator = AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIterator; ++SenseConfigIterator) {
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIterator)->GetSenseImplementation(), true);
	}
}

void ALOLAIController::PawnDeadTagUpdated(const FGameplayTag Tag, int32 Count)
{
	if (Count != 0) {
		GetBrainComponent()->StopLogic("Dead");
		ClearAndDisableAllSenses();
		bIsPawnDead = true;
	}
	else {
		GetBrainComponent()->StartLogic();
		EnableAllSenses();
		bIsPawnDead = false;
	}
}

void ALOLAIController::PawnStunTagUpdated(const FGameplayTag Tag, int32 Count)
{
	if (bIsPawnDead) return;
	if (Count != 0) {
		GetBrainComponent()->StopLogic("Stun");
	}
	else {
		GetBrainComponent()->StartLogic();
	}
}
