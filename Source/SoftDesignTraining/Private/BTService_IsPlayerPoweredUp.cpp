// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_IsPlayerPoweredUp.h"
#include "../SDTAIController.h"
#include "../SDTUtils.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

UBTService_IsPlayerPoweredUp::UBTService_IsPlayerPoweredUp()
{
    bCreateNodeInstance = true;
}

void UBTService_IsPlayerPoweredUp::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent())
    {
        if (ASDTAIController* aiController = Cast<ASDTAIController>(OwnerComp.GetAIOwner()))
        {
            ASoftDesignTrainingMainCharacter* PlayerCharacter = Cast<ASoftDesignTrainingMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
            bool bIsPoweredUp = PlayerCharacter->IsPoweredUp();
            OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Bool>("IsPlayerPoweredUp", bIsPoweredUp);
        }
    }
}