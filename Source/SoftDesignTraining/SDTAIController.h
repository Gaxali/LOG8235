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

    /////
    void AddMovement(FVector NewDirection);
    bool TestRaycast(float AngleDegrees, float Length, FVector& ImpactNormalOutput);
    void NavigationPatrol();
    /////

public:

    ASDTAIController(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;

    virtual void Tick(float deltaTime) override;

private:

    UPROPERTY()
    TObjectPtr<USDTStateMachine> StateMachine;

};
