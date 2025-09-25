// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "SDTAIController.generated.h"

class USDTStateMachine;
class ACharacter;

UENUM(BlueprintType)
enum class AIState : uint8
{
    Patrol,
    Chase,
    Flee,
    Collect
};

UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public AAIController
{
    GENERATED_BODY()

    void AddMovementSides(const FHitResult& ResultIn, float Direction);
    void AvoidingObstaces();
    void SpeedAdjustment();
    /////
    void AddMovement(FVector NewDirection);
    bool TestRaycast(float AngleSideDegrees, float AngleDegreesDown, float Length, FHitResult& ResultOut, FColor Color = FColor::Red);
    bool IsCharacterClose(ACharacter* TargetCharacter);
    bool IsCharacterInSight(ACharacter* TargetCharacter);
    void FindPickup();
    void UpdateReferencePosition(ACharacter* TargetCharacter);
	void Transition();
    void NavigationPatrol();
    void Chase();
    void Flee();
    void Move();
    void Collect();
    /////

	bool HitObjectByChannelName(FHitResult& Result, FName& ChannelName);

public:

    ASDTAIController(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;

    virtual void Tick(float deltaTime) override;

private:

    UPROPERTY()
    TObjectPtr<USDTStateMachine> StateMachine;
	AIState CurrentState = AIState::Patrol;
	FVector ReferencePlayerPosition;
    FVector ClosestPickupPosition = FVector::ZeroVector;;

    UPROPERTY()
    ACharacter* CharacterAI;

    float MaxWalkSpeed = 500.0f;
};
