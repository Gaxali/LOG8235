// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsPlayerSeen.h"
#include "../SDTAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "Kismet/GameplayStatics.h"

UBTService_IsPlayerSeen::UBTService_IsPlayerSeen()
{
    bCreateNodeInstance = true;
}

void UBTService_IsPlayerSeen::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
    {
        aiController->PlayerInteractionLoSUpdate();
                
        OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), aiController->IsTargetPlayerSeen());
    }
}
