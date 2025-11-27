// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TacticalPositionAssigned.h"
#include "../SDTAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"


EBTNodeResult::Type UBTTask_TacticalPositionAssigned::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

    if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
    {
        if (OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID()))
        {
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(aiController->GetTargetPosBBKeyID(), aiController->TargetPos);
            
            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}