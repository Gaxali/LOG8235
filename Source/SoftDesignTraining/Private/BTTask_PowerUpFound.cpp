// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PowerUpFound.h"
#include "../SDTAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "Kismet/GameplayStatics.h"
#include "../SDTCollectible.h"
#include "../SDTUtils.h"

EBTNodeResult::Type UBTTask_PowerUpFound::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    float closestSqrCollectibleDistance = 18446744073709551610.f;
    ASDTCollectible* closestCollectible = nullptr;

    TArray<AActor*> foundCollectibles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASDTCollectible::StaticClass(), foundCollectibles);

    if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
    {
        if (!aiController->m_ReachedTarget)
        {
            return EBTNodeResult::Succeeded;
        }

        //if (aiController->m_ReachedTarget)
        //{
        //    PowerUpAssigned = 0;
        //}
        //
        //if (PowerUpAssigned == 1)
        //{
        //    return EBTNodeResult::Succeeded;
        //}
    }

    while (foundCollectibles.Num() != 0)
    {
        int index = FMath::RandRange(0, foundCollectibles.Num() - 1);

        ASDTCollectible* collectibleActor = Cast<ASDTCollectible>(foundCollectibles[index]);
        if (!collectibleActor)
            EBTNodeResult::Failed;

        if (!collectibleActor->IsOnCooldown())
        {
            //MoveToLocation(foundCollectibles[index]->GetActorLocation(), 0.5f, false, true, true, false, NULL, false);
            //OnMoveToTarget();
            auto pos = foundCollectibles[index]->GetActorLocation();
            UE_LOG(LogTemp, Warning, TEXT("Position: X=%f  Y=%f  Z=%f"), pos.X, pos.Y, pos.Z);
            OwnerComp.GetBlackboardComponent()->SetValueAsVector("TargetPos", foundCollectibles[index]->GetActorLocation());
            //        return EBTNodeResult::Succeeded;
            
            if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
                aiController->OnMoveToTarget();

            //PowerUpAssigned = 1;

            return EBTNodeResult::Succeeded;
        }
        else
        {
            foundCollectibles.RemoveAt(index);
        }
    }

    return EBTNodeResult::Failed;

    //APawn* controlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
    //if (!controlledPawn)
    //    return EBTNodeResult::Failed;;
    //
    //UWorld* world = controlledPawn->GetWorld();
    //if (!world)
    //    return EBTNodeResult::Failed;;
    //
    //TArray<AActor*> foundCollectibles;
    //UGameplayStatics::GetAllActorsOfClass(world, ASDTCollectible::StaticClass(), foundCollectibles);
    //
    //for (AActor* actor : foundCollectibles)
    //{
    //    ASDTCollectible* collectible = Cast<ASDTCollectible>(actor);
    //    if (!collectible || collectible->IsOnCooldown())
    //        continue;
    //
    //    FVector source = controlledPawn->GetActorLocation();
    //    FVector target = collectible->GetActorLocation();
    //
    //    if (SDTUtils::Raycast(world, source, target))
    //    {
    //        OwnerComp.GetBlackboardComponent()->SetValueAsVector("TargetPos", target);
    //        return EBTNodeResult::Succeeded;
    //    }
    //}
    //
    //return EBTNodeResult::Failed;

}
