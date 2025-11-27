// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_IsPlayerSeen.h"
#include "../SDTAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UBTService_IsPlayerSeen::UBTService_IsPlayerSeen()
{
    bCreateNodeInstance = true;
}

void UBTService_IsPlayerSeen::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
    {
        aiController->PlayerInteractionLoSUpdate();

        if (aiController->IsTargetPlayerSeen())
        {
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), true);
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(aiController->GetTargetPosBBKeyID(), aiController->GetTargetPlayerPos());
            
            if (aiController->m_ReachedTarget)
            {
                OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetArrivedToTacticalPositionID(), true);
            }
            //OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetIsSelectedForTacticalGroupID(), true);
        }
        //else
        //{
        //    OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), false);
        //}
        //if (aiBase->IsTargetPlayerSeen())
        //{
        //    //write to bb that the player is seen
        //    OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), true);
        //
        //    //write to bb the position of the target
        //    OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(aiController->GetTargetPosBBKeyID(), aiBase->GetTargetPlayerPos());
        //}
        //else
        //{
        //    //write to bb that the player is not seen
        //    OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>(aiController->GetTargetSeenKeyID(), false);
        //}
    }
}
