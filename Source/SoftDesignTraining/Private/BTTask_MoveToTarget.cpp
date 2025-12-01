// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToTarget.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "../SDTAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{    
    FVector targetPosition = FVector::ZeroVector;

    if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
    {
        if (aiController->m_ReachedTarget)
        {
            aiController->OnMoveToTarget();
            auto FinalTargetPosForMoveTo = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Vector>(aiController->GetTargetPosBBKeyID());
            UAIBlueprintHelperLibrary::SimpleMoveToLocation(aiController, FinalTargetPosForMoveTo);
        }
        //if (!aiController->InAir)
        //{
        //    targetPosition = OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Vector>(aiController->GetTargetPosBBKeyID());

         //   UAIBlueprintHelperLibrary::SimpleMoveToLocation(aiController, targetPosition);

        //}

        return EBTNodeResult::Succeeded;
    }
    
    return EBTNodeResult::Failed;
}