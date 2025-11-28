// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsPowerUpSeen.h"
#include "../SDTAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

UBTService_IsPowerUpSeen::UBTService_IsPowerUpSeen()
{
    bCreateNodeInstance = true;
}

void UBTService_IsPowerUpSeen::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    APawn* controlledPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
    if (!controlledPawn)
        return;

    UWorld* world = controlledPawn->GetWorld();
    if (!world)
        return;

    TArray<AActor*> foundCollectibles;
    UGameplayStatics::GetAllActorsOfClass(world, ASDTCollectible::StaticClass(), foundCollectibles);

    for (AActor* actor : foundCollectibles)
    {
        ASDTCollectible* collectible = Cast<ASDTCollectible>(actor);
        if (!collectible || collectible->IsOnCooldown())
            continue;

        FVector source = controlledPawn->GetActorLocation();
        FVector target = collectible->GetActorLocation();

        if (SDTUtils::Raycast(world, source, target))
        {
            OwnerComp.GetBlackboardComponent()->SetValueAsVector("TargetPos", target);
			OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsPowerUpSeen", true);
            return; 
        }
    }
}