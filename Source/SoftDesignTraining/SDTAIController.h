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

    void Navigation(const FVector& Direction);

    void AddMovement(FVector NewDirection);
    bool TestRaycast(float AngleSideDegrees, float AngleDegreesDown, float Length, FHitResult& ResultOut, FColor Color = FColor::Red);
    bool HitObjectByChannelName(FHitResult& Result);

public:

    ASDTAIController(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;

    virtual void Tick(float deltaTime) override;

protected:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
    FName ChannelName = TEXT("DeathObject");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
    float AngleSideDegree = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
    float AngleDownDegree = 35.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Navigation")
    float MaxWalkSpeed = 500.0f;

private:

    UPROPERTY()
    TObjectPtr<USDTStateMachine> StateMachine;
	
    UPROPERTY()
    ACharacter* CharacterAI;

    FCollisionObjectQueryParams QueryParams;

    
};
