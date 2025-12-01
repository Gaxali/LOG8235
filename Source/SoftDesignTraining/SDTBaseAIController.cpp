// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTBaseAIController.h"
#include "SoftDesignTraining.h"

ASDTBaseAIController::ASDTBaseAIController(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void ASDTBaseAIController::BeginPlay()
{
    Super::BeginPlay();
}

void ASDTBaseAIController::OnPossess(APawn* pawn)
{
    Super::OnPossess(pawn);
}

void ASDTBaseAIController::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    UpdatePlayerInteraction(deltaTime);

    //if (m_ReachedTarget)
    //{
        //GoToBestTarget(deltaTime);
    //}
    //else
    //{
    //    ShowNavigationPath();
    //}
}


