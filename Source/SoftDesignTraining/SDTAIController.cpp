// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"

#include "Algo/RandomShuffle.h"
#include "HAL/PlatformTime.h" // for FPlatformTime::Cycles

#include "SDTStateMachine.h"

ASDTAIController::ASDTAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    StateMachine = CreateDefaultSubobject<USDTStateMachine>("AIStateMachine");
}

void ASDTAIController::BeginPlay()
{
    Super::BeginPlay();

    auto AIPawn = GetPawn();

    if (AIPawn)
    {
        CharacterAI = Cast<ACharacter>(AIPawn);
        CharacterAI->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
    }
}

bool ASDTAIController::TestRaycast(float AngleSideDegrees, float AngleDegreesDown, float Length, FHitResult& ResultOut, FColor Color)
{
    bool Success = false;

    FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllStaticObjects);
    
    auto ForwardVector = GetPawn()->GetActorForwardVector();

    FVector Up(0, 0, 1);
    FVector VectorRotated = ForwardVector.RotateAngleAxis(AngleSideDegrees, Up);
    VectorRotated = VectorRotated.RotateAngleAxis(AngleDegreesDown, FVector::CrossProduct(Up, VectorRotated));

    auto PositionStart = GetPawn()->GetActorLocation();
    auto PositionEnd = GetPawn()->GetActorLocation() + Length * VectorRotated;

    Success = GetWorld()->LineTraceSingleByObjectType(ResultOut,
        PositionStart,
        PositionEnd,
        QueryParams);

    DrawDebugDirectionalArrow(GetWorld(), PositionStart, PositionEnd, 2, Color, false);

    return Success;
}

void ASDTAIController::AddMovement(FVector NewDirection)
{
    GetPawn()->AddMovementInput(NewDirection, 1);
    GetPawn()->SetActorRotation(NewDirection.ToOrientationQuat());
}

void ASDTAIController::AddMovementSides(const FHitResult& ResultIn, float Direction)
{
    int Factor = FMath::RandRange(0, 20) / 100;

    auto TangenDirection = Direction * FVector::CrossProduct(GetPawn()->GetActorUpVector(), ResultIn.ImpactNormal);
    auto NewDirection = (TangenDirection + ResultIn.ImpactNormal * Factor).GetSafeNormal();
    AddMovement(NewDirection);
}

void ASDTAIController::AvoidingObstaces()
{
    FHitResult Result1;
    FHitResult Result2;
    
    float Angle = 20.0f;

    bool CollisionDetectionLeftRay = TestRaycast(-Angle, 35, 200, Result1);
    bool CollisionDetectionRightRay = TestRaycast(Angle, 35, 200, Result2);

    if (CollisionDetectionLeftRay && CollisionDetectionRightRay)
    {
        FVector TotalDirection = (Result1.ImpactNormal + Result2.ImpactNormal).GetSafeNormal();

        int AngleDeg = FMath::RandRange(-10, 10);
        FVector Up(0, 0, 1);
        FVector VectorRotated = TotalDirection.RotateAngleAxis(AngleDeg, Up);

        AddMovement(VectorRotated);
    }
    else if (CollisionDetectionLeftRay)
    {
        AddMovementSides(Result1, -1);

    }
    else if (CollisionDetectionRightRay)
    {
        AddMovementSides(Result2, 1);
    }
    else
    {
        AddMovement(GetPawn()->GetActorForwardVector());
    }
}

void ASDTAIController::SpeedAdjustment()
{
    if (!CharacterAI)
    {
        return;
    }

    FHitResult Result;
    float MaxLenghtOfTheRayCast = 300.0f;
    float Distance = MaxLenghtOfTheRayCast;

    if (TestRaycast(0, 0, 300, Result, FColor::Blue))
    {
        Distance = Result.Distance;

    }

    CharacterAI->GetCharacterMovement()->MaxWalkSpeed = (Distance / MaxLenghtOfTheRayCast) * MaxWalkSpeed;
}

void ASDTAIController::NavigationPatrol()
{
    AvoidingObstaces();

    SpeedAdjustment();

}

void ASDTAIController::Tick(float deltaTime)
{
	NavigationPatrol();
    //UE_LOG(LogTemp, Warning, TEXT("MyValue is: %f , %f, %f"), 0.0f, 0.0f, 0.0f);

	Super::Tick(deltaTime);

    if (StateMachine)
    {
        StateMachine->Run();
    }
}




