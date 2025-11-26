// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SoftDesignTrainingGameMode.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTrainingPlayerController.h"
#include "SoftDesignTrainingCharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"

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
}

void ASoftDesignTrainingGameMode::OnQueryTacticalPositionsAttack(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if (Locations.IsValidIndex(0))//Locations.Num() > 0)
	{
		//GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

		//DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
	}
}

void ASoftDesignTrainingGameMode::PlayerSeenByAI(ASDTAIController* InstigatorAIController)
{
	if (!PlayerSeenBroadcasted)
	{
		if (TacticalAttackPositionsQuery)
		{
			UEnvQueryInstanceBlueprintWrapper* QueryInst = UEnvQueryManager::RunEQSQuery(this, TacticalAttackPositionsQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
			OnPayerSeenChange.Broadcast(true);
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
}