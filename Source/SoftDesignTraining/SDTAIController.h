// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "SDTAIController.generated.h"

class USDTStateMachine;
/**
 * 
 */
UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public AAIController
{
    GENERATED_BODY()

    void AvoidObstacle();
    bool AddMovementToAvoidObstacle(float AngleDegrees, float Length);

public:

    ASDTAIController(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;

    virtual void Tick(float deltaTime) override;

private:

    UPROPERTY()
    TObjectPtr<USDTStateMachine> StateMachine;

    TArray<float> V;      // {1,2,3}
    FRandomStream RNG;    // deterministic stream (seeded in BeginPlay)
};
