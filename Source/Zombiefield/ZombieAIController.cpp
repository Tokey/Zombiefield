// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieAIController.h"
#include "AICharacter.h"
#include "Waypoint.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "MainCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


AZombieAIController::AZombieAIController() {
	PrimaryActorTick.bCanEverTick = true;

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));

	SightConfig->SightRadius = AISightRadius;
	SightConfig->LoseSightRadius = AILoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = AIFieldOfView;
	SightConfig->SetMaxAge(AISightAge);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AZombieAIController::OnPawnDetected);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
}

void AZombieAIController::BeginPlay()
{
	Super::BeginPlay();

	if (GetPerceptionComponent() != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("All Systems Set"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Some Problem Occured"));
	}

}

void AZombieAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AZombieAIController::OnUnPossess()
{
}

void AZombieAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AAICharacter* AICharacter = Cast<AAICharacter>(GetPawn());
	if (AICharacter != nullptr)
	{
		/*AMainCharacter* Playerr = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));;
		if (DistanceToPlayer == 0)
		{
			if (Playerr != nullptr)
				Playerr->Health--;
		}
		if (Playerr != nullptr)
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, FString::Printf(TEXT("health: %d"), Playerr->Health));
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, FString::Printf(TEXT("DISTANCE: %d"), DistanceToPlayer));*/

		if (DistanceToPlayer > AISightRadius)
		{
			bIsPlayerDetected = false;
		}

		if (AICharacter->NextWaypoint != nullptr && bIsPlayerDetected != true)
		{
			MoveToActor(AICharacter->NextWaypoint, 5.0f);
		}
		else if (bIsPlayerDetected == true)
		{
			AMainCharacter* Player = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			MoveToActor(Player, 5.0f);

			/*if (AICharacter->GetVelocity() == FVector(0,0,0))
				UGameplayStatics::OpenLevel(GetWorld(), FName("FirstPersonExampleMap"));*/
			
		}
		
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, FString::Printf(TEXT("NOT GETTING AI NIBBA")));
	}

	
}

FRotator AZombieAIController::GetControlRotation() const
{
	if (GetPawn() == nullptr)
	{
		return FRotator(0.0f, 0.0f, 0.0f);
	}

	return FRotator(0.0f, GetPawn()->GetActorRotation().Yaw, 0.0f);

}

void AZombieAIController::OnPawnDetected(const TArray<AActor*>& DetectedPawns)
{
	for (size_t i = 0; i < DetectedPawns.Num(); i++)
	{
		DistanceToPlayer = GetPawn()->GetDistanceTo(DetectedPawns[i]);
		
		
		
		//UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), DistanceToPlayer);
	}

	bIsPlayerDetected = true;
}
