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
}

void ASoftDesignTrainingGameMode::OnQueryTacticalPositionsAttack(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
		
	for (TActorIterator<ASoftDesignTrainingCharacter> It(GetWorld()); It; ++It)
	{
		ASoftDesignTrainingCharacter* Bot = *It;
			
		ASDTAIController* AIController = Cast<ASDTAIController>(Bot->GetController());

		if (AIController)
		{
			if (Locations.Num() > 0)
			{
				AIController->TargetPos = Locations.Pop(EAllowShrinking::Yes);
				DrawDebugSphere(GetWorld(), AIController->TargetPos, 50.0f, 20, FColor::Blue, false, 60.0f);
			}
				
		}
		//Bot->GetController()

		//USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);

		//if (ensure(AttributeComp) && AttributeComp->IsAlive())
		//{
		//	AttributeComp->Kill(this); //@fixme: pass in player? for kill credit
		//}
	}

	OnPayerSeenChange.Broadcast(true);

	//GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

	//DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
	
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
}