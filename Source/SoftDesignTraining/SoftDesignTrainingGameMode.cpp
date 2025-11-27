// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SoftDesignTrainingGameMode.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTrainingPlayerController.h"
#include "SoftDesignTrainingCharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EngineUtils.h" // for TActorIterator<>
#include "SDTAIController.h"

ASoftDesignTrainingGameMode::ASoftDesignTrainingGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ASoftDesignTrainingPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/BP_SDTMainCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SoftPlayerController = nullptr;
}

void ASoftDesignTrainingGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (FVector::DistSquared(PlayerLKP, SoftPlayerController->GetPawn()->GetActorLocation()) > 25.0)
	{
		if (TacticalAttackPositionsQuery && PlayerSeenBroadcasted)
		{
			UEnvQueryInstanceBlueprintWrapper* QueryInst = UEnvQueryManager::RunEQSQuery(this, TacticalAttackPositionsQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
			// Can run in multiple frames
			QueryInst->GetOnQueryFinishedEvent().AddDynamic(this, &ASoftDesignTrainingGameMode::OnQueryTacticalPositionsAttack);
		}

		PlayerLKP = SoftPlayerController->GetPawn()->GetActorLocation();
	}

	// Your logic here
}

void ASoftDesignTrainingGameMode::OnQueryTacticalPositionsAttack(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	
	// Sort AI array base on player position//
	TArray<ASDTAIController*> AIControllers;

	for (TActorIterator<ASoftDesignTrainingCharacter> It(GetWorld()); It; ++It)
	{
		ASoftDesignTrainingCharacter* Bot = *It;

		ASDTAIController* AIController = Cast<ASDTAIController>(Bot->GetController());

		if (AIController)
		{
			AIControllers.Add(AIController);
		}
	}

	if (SoftPlayerController)
	{
		PlayerLKP = SoftPlayerController->GetPawn()->GetActorLocation();

		AIControllers.Sort([PlayerLKP = PlayerLKP](ASDTAIController& A, ASDTAIController& B)
			{
				return FVector::DistSquared(A.GetPawn()->GetActorLocation(), PlayerLKP) < FVector::DistSquared(B.GetPawn()->GetActorLocation(), PlayerLKP);
			});
	}
	///////////////////////////////////////////

	for (ASDTAIController* AIController : AIControllers)
	{
		auto AIPosition = AIController->GetPawn()->GetActorLocation();

		if (Locations.Num() > 0)
		{
			Locations.Sort([AIPosition](const FVector& A, const FVector& B)
			{
				return FVector::DistSquared(A, AIPosition) < FVector::DistSquared(B, AIPosition);
			});

			FVector Closest = Locations[0];

			// Remove it
			Locations.RemoveAt(0);

			AIController->TargetPos = Closest;
			DrawDebugSphere(GetWorld(), Closest, 50.0f, 10, FColor::Blue, false, 0.5f);
		}
		else
		{
			AIController->TargetPos = AIPosition;
		}
		
	}

	OnPayerSeenChange.Broadcast(true);
}

void ASoftDesignTrainingGameMode::PlayerSeenByAI(ASDTAIController* InstigatorAIController)
{
	if (!PlayerSeenBroadcasted)
	{
		if (TacticalAttackPositionsQuery)
		{
			UEnvQueryInstanceBlueprintWrapper* QueryInst = UEnvQueryManager::RunEQSQuery(this, TacticalAttackPositionsQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
			PlayerSeenBroadcasted = true;

			// Can run in multiple frames
			QueryInst->GetOnQueryFinishedEvent().AddDynamic(this, &ASoftDesignTrainingGameMode::OnQueryTacticalPositionsAttack);
		}
	}
}

void ASoftDesignTrainingGameMode::StartPlay()
{
    Super::StartPlay();

    GetWorld()->Exec(GetWorld(), TEXT("stat fps"));

	for (TActorIterator<ASoftDesignTrainingCharacter> It(GetWorld()); It; ++It)
	{
		ASoftDesignTrainingPlayerController* PlayerController = Cast<ASoftDesignTrainingPlayerController>((*It)->GetController());

		if (PlayerController)
			SoftPlayerController = PlayerController;
	}
}