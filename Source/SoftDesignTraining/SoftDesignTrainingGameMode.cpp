// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SoftDesignTrainingGameMode.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTrainingPlayerController.h"
#include "SoftDesignTrainingCharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EngineUtils.h" // for TActorIterator<>
#include "SDTAIController.h"
#include "SoftDesignTrainingMainCharacter.h"

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

	if (AIControllersTacticalGroup.IsEmpty())
		return;

	if (!SoftPlayerController)
		return;

	//Update Tactical group
	if (FVector::DistSquared(PlayerLKP, SoftPlayerController->GetPawn()->GetActorLocation()) > 250000.0)
	{
		RunEQSForTacticalGroupCreation();

		PlayerLKP = SoftPlayerController->GetPawn()->GetActorLocation();
	}

	for (ASDTAIController* AIController : AIControllersTacticalGroup)
	{
		FVector npcPosition = AIController->GetPawn()->GetActorLocation();
		FVector npcHead = npcPosition + FVector::UpVector * 200.0f;
		UWorld* npcWorld = GetWorld();

		DrawDebugSphere(npcWorld, npcHead, 20.0f, 32, FColor::Magenta);
	}
	// Your logic here
}

void ASoftDesignTrainingGameMode::RemoveFromGroupWhenDie(ASDTAIController* InstigatorAIController)
{
	if (InstigatorAIController)
	{
		InstigatorAIController->SetIsTargetPlayerSeen(false);
		AIControllersTacticalGroup.Remove(InstigatorAIController);
	}
	
}


void ASoftDesignTrainingGameMode::PlayerPickUpPowerUp()
{
	for (ASDTAIController* AIController : AIControllersTacticalGroup)
	{
		AIController->SetIsTargetPlayerSeen(false);
	}

	AIControllersTacticalGroup.Empty();

	//OnPlayerPowerUp.Broadcast(true);
}

void ASoftDesignTrainingGameMode::RunEQSForTacticalGroupCreation()
{
	if (TacticalAttackPositionsQuery)
	{
		UEnvQueryInstanceBlueprintWrapper* QueryInst = UEnvQueryManager::RunEQSQuery(this, TacticalAttackPositionsQuery, this, EEnvQueryRunMode::AllMatching, nullptr);

		// Can run in multiple frames
		QueryInst->GetOnQueryFinishedEvent().AddDynamic(this, &ASoftDesignTrainingGameMode::OnQueryTacticalPositionsAttack);
	}
}

//void ASoftDesignTrainingGameMode::SortTacticalGroupBasePlayerLKP()
//{
//	AIControllersTacticalGroup.Sort([PlayerLKP = PlayerLKP](ASDTAIController& A, ASDTAIController& B)
//		{
//			return FVector::DistSquared(A.GetPawn()->GetActorLocation(), PlayerLKP) < FVector::DistSquared(B.GetPawn()->GetActorLocation(), PlayerLKP);
//		});
//}

void ASoftDesignTrainingGameMode::OnQueryTacticalPositionsAttack(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	
	if (SoftPlayerController)
	{
		PlayerLKP = SoftPlayerController->GetPawn()->GetActorLocation();
	}

	TArray<ASDTAIController*> GroupTemporal;

	//if (AIControllersTacticalGroup.IsEmpty())
	//{
	for (TActorIterator<ASoftDesignTrainingCharacter> It(GetWorld()); It; ++It)
	{
		ASoftDesignTrainingCharacter* Bot = *It;

		ASDTAIController* AIController = Cast<ASDTAIController>(Bot->GetController());

		if (AIController)
		{
			GroupTemporal.Add(AIController);
		}
	}
	//}
	// Sort group base on player LKP
	GroupTemporal.Sort([PlayerLKP = PlayerLKP](ASDTAIController& A, ASDTAIController& B)
		{
			return FVector::DistSquared(A.GetPawn()->GetActorLocation(), PlayerLKP) < FVector::DistSquared(B.GetPawn()->GetActorLocation(), PlayerLKP);
		});

	// keep the closest to the player
	int32 Count = FMath::Min(Locations.Num(), GroupTemporal.Num());

	for (int32 i = 0; i < Count; ++i)
	{
		ASDTAIController* Controller = GroupTemporal[i];

		if (!AIControllersTacticalGroup.Contains(Controller))
		{
			AIControllersTacticalGroup.Insert(Controller, 0); // .Add(Controller);
		}
	}

	// Set TargetPos for all
	//for (ASDTAIController* AIController : GroupTemporal)
	//{
	//	auto AIPosition = AIController->GetPawn()->GetActorLocation();
	//	AIController->TargetPos = AIPosition;
	//}

	// Assing tactical positions to the group
	for (ASDTAIController* AIController : AIControllersTacticalGroup)
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

			AIController->TActicalPos = Closest;
			//AIController->m_ReachedTarget = 
			AIController->AIStateInterrupted();
			DrawDebugSphere(GetWorld(), Closest, 50.0f, 10, FColor::Blue, false, 0.5f);
		}
		else
		{
			AIController->TActicalPos = AIPosition;
			//AIController->m_ReachedTarget = 
			AIController->AIStateInterrupted();
		}
		
		AIController->SetIsTargetPlayerSeen(true);
	}

	//OnPayerSeenChange.Broadcast(true);
}

void ASoftDesignTrainingGameMode::PlayerSeenByAI(ASDTAIController* InstigatorAIController)
{
	// Prevent creattion when the Bots are in flee mode. The raycast can still detect the player
	// when moving to flee position

	if (SoftPlayerController)
	{
		if (ASoftDesignTrainingMainCharacter* Player = Cast<ASoftDesignTrainingMainCharacter>(SoftPlayerController->GetCharacter()))
		{
			if (Player->IsPoweredUp())
			{
				//UE_LOG(LogTemp, Warning, TEXT("=== AIControllers ==="));
				return;
			}
		}
	}

	if (AIControllersTacticalGroup.IsEmpty())
	{
		RunEQSForTacticalGroupCreation();
	}
	else
	{
		//if (!AIControllersTacticalGroup.Contains(InstigatorAIController))
		//{
		//	if (SoftPlayerController)
		//	{
		//		auto PlayerPos = SoftPlayerController->GetPawn()->GetActorLocation();
		//
		//		if (FVector::DistSquared(PlayerPos, InstigatorAIController->GetPawn()->GetActorLocation()) < 250000.0)
		//		{
		//			auto AIPosition = InstigatorAIController->GetPawn()->GetActorLocation();
		//			InstigatorAIController->TargetPos = AIPosition;
		//			InstigatorAIController->SetIsTargetPlayerSeen(true);
		//
		//			AIControllersTacticalGroup.Add(InstigatorAIController);
		//		}
		//	}
		//}
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