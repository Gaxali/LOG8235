// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "SDTAIController.generated.h"

class USDTStateMachine;
class ACharacter;

UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public AAIController
{
    GENERATED_BODY()

    void AddMovementSides(const FHitResult& ResultIn, float Direction);
    void AvoidingObstaces(const FVector& Direction);
    void SpeedAdjustment();

    void Navigation(const FVector& Direction);

    void AddMovement(FVector NewDirection);
    bool TestRaycast(float AngleSideDegrees, float AngleDegreesDown, float Length, FHitResult& ResultOut, FColor Color = FColor::Red);
    bool HitObjectByChannelName(FHitResult& Result, FName& ChannelName);

public:

    ASDTAIController(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;

    virtual void Tick(float deltaTime) override;

private:

    UPROPERTY()
    TObjectPtr<USDTStateMachine> StateMachine;
	
    UPROPERTY()
    ACharacter* CharacterAI;

    float MaxWalkSpeed = 500.0f;
};
