// Fill out your copyright notice in the Description page of Project Settings.

#include "../SDTFleeLocation.h"
#include "Kismet/KismetMathLibrary.h"
#include "BTTask_FindFleeLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../SDTAIController.h"
#include "EngineUtils.h"

EBTNodeResult::Type UBTTask_FindFleeLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* aiController = OwnerComp.GetAIOwner();
    APawn* controlledPawn = aiController ? aiController->GetPawn() : nullptr;
    UWorld* world = controlledPawn ? controlledPawn->GetWorld() : nullptr;

    if (!world || !controlledPawn)
        return EBTNodeResult::Failed;

    float bestLocationScore = 0.f;
    ASDTFleeLocation* bestFleeLocation = nullptr;

    ACharacter* playerCharacter = UGameplayStatics::GetPlayerCharacter(world, 0);
    if (!playerCharacter)
        return EBTNodeResult::Failed;

    for (TActorIterator<ASDTFleeLocation> actorIterator(world, ASDTFleeLocation::StaticClass()); actorIterator; ++actorIterator)
    {
        ASDTFleeLocation* fleeLocation = Cast<ASDTFleeLocation>(*actorIterator);
        if (fleeLocation)
        {
            float distToFleeLocation = FVector::Dist(fleeLocation->GetActorLocation(), playerCharacter->GetActorLocation());

            FVector selfToPlayer = playerCharacter->GetActorLocation() - controlledPawn->GetActorLocation();
            selfToPlayer.Normalize();

            FVector selfToFleeLocation = fleeLocation->GetActorLocation() - controlledPawn->GetActorLocation();
            selfToFleeLocation.Normalize();

            float fleeLocationToPlayerAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(selfToPlayer, selfToFleeLocation)));
            float locationScore = distToFleeLocation + fleeLocationToPlayerAngle * 100.f;

            if (locationScore > bestLocationScore)
            {
                bestLocationScore = locationScore;
                bestFleeLocation = fleeLocation;
            }

            DrawDebugString(world, FVector(0.f, 0.f, 10.f), FString::SanitizeFloat(locationScore), fleeLocation, FColor::Red, 5.f, false);
        }
    }

    if (bestFleeLocation)
    {
        if (UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent())
        {
            MyBlackboard->SetValueAsObject("TargetPos", bestFleeLocation);
        }
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::Failed;
}