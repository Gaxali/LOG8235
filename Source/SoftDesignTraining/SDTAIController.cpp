// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"

#include "Algo/RandomShuffle.h"
#include "HAL/PlatformTime.h" // for FPlatformTime::Cycles

void ASDTAIController::BeginPlay()
{
    Super::BeginPlay();

    auto AIPawn = GetPawn();

    if (AIPawn)
    {
        ACharacter* CharacterAI = Cast<ACharacter>(AIPawn);
        CharacterAI->GetCharacterMovement()->MaxWalkSpeed = 500.0f;
    }

    V = { -30, 30 };
    uint64 Seed = FPlatformTime::Cycles64() ^ (uint64)this;
    RNG.Initialize((int32)(Seed & 0xFFFFFFFF));
}

bool ASDTAIController::AddMovementToAvoidObstacle(float AngleDegrees, float Length)
{
    FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllStaticObjects);

    FHitResult Result;

    auto ForwardVector = GetPawn() -> GetActorForwardVector();

    FVector Up(0, 0, 1);
    FVector VectorRotated = ForwardVector.RotateAngleAxis(AngleDegrees, Up);

    auto PositionStart = GetPawn()->GetActorLocation();
    auto PositionEnd = GetPawn()->GetActorLocation() + Length * VectorRotated;
    
    bool bHitWorld = GetWorld()->LineTraceSingleByObjectType(Result,
        PositionStart,
        PositionEnd,
        QueryParams);

    if (bHitWorld)
    {
        float SignAngle = FMath::Sign(AngleDegrees);
        auto NewDirection = SignAngle * FVector::CrossProduct(GetPawn()->GetActorUpVector(), Result.ImpactNormal);
        GetPawn()->AddMovementInput(NewDirection, 1);
        GetPawn()->SetActorRotation(NewDirection.ToOrientationQuat());

        DrawDebugDirectionalArrow(GetWorld(), PositionStart, PositionEnd, 2, FColor::Red, false);
        //GetPawn()->AddMovementInput(ResultFront.ImpactNormal, 1);
        return true;
    }

    return false;
}

void ASDTAIController::AvoidObstacle()
{
    for (int32 i = V.Num() - 1; i > 0; --i)
    {
        const int32 j = RNG.RandRange(0, i);
        V.Swap(i, j);
    }

    bool IsAvoiding = AddMovementToAvoidObstacle(V[0], 100);

    if (!IsAvoiding)
        IsAvoiding = AddMovementToAvoidObstacle(V[1], 80);

    //if (!IsAvoiding)
    //    IsAvoiding = AddMovePerpendicularToObstacle(V[2], 70);

    if (!IsAvoiding)
    {
        GetPawn()->AddMovementInput(GetPawn()->GetActorForwardVector(), 1);
    }
}

void ASDTAIController::Tick(float deltaTime)
{
	AvoidObstacle();

    //UE_LOG(LogTemp, Warning, TEXT("MyValue is: %f , %f, %f"), 0.0f, 0.0f, 0.0f);

	Super::Tick(deltaTime);
}




