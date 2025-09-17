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
        ACharacter* CharacterAI = Cast<ACharacter>(AIPawn);
        CharacterAI->GetCharacterMovement()->MaxWalkSpeed = 500.0f;
    }
}

bool ASDTAIController::TestRaycast(float AngleDegrees, float Length, FVector& ImpactNormalOutput)
{
    bool Success = false;

    FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllStaticObjects);
    FHitResult Result;

    auto ForwardVector = GetPawn()->GetActorForwardVector();

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
        ImpactNormalOutput = Result.ImpactNormal;
       
        Success = true;
    }

    DrawDebugDirectionalArrow(GetWorld(), PositionStart, PositionEnd, 2, FColor::Red, false);

    return Success;
}

void ASDTAIController::AddMovement(FVector NewDirection)
{
    GetPawn()->AddMovementInput(NewDirection, 1);
    GetPawn()->SetActorRotation(NewDirection.ToOrientationQuat());
}

void ASDTAIController::NavigationPatrol()
{
    FVector ImpactNormal1 = FVector::Zero();
    FVector ImpactNormal2 = FVector::Zero();

    float Angle = 30.0f;

    bool CollisionDetectionLeftRay = TestRaycast(-Angle, 150, ImpactNormal1);
    bool CollisionDetectionRightRay = TestRaycast(Angle, 150, ImpactNormal2);
    
    if (CollisionDetectionLeftRay && CollisionDetectionRightRay)
    {
        FVector TotalDirection = (ImpactNormal1 + ImpactNormal2).GetSafeNormal();

        int AngleDeg = FMath::RandRange(-10, 10);
        FVector Up(0, 0, 1);
        FVector VectorRotated = TotalDirection.RotateAngleAxis(AngleDeg, Up);

        AddMovement(VectorRotated);
    }
    else if (CollisionDetectionLeftRay)
    {
        int Factor = FMath::RandRange(0, 20) / 100;

        auto TangenDirection = -FVector::CrossProduct(GetPawn()->GetActorUpVector(), ImpactNormal1);
        auto NewDirection = (TangenDirection + ImpactNormal1 * Factor).GetSafeNormal();
        AddMovement(NewDirection);

    }
    else if (CollisionDetectionRightRay)
    {
        int Factor = FMath::RandRange(0, 20) / 100;
        auto TangenDirection = FVector::CrossProduct(GetPawn()->GetActorUpVector(), ImpactNormal2);
        auto NewDirection = (TangenDirection + ImpactNormal2 * Factor).GetSafeNormal();
        AddMovement(NewDirection);
    }
    else
    {
        AddMovement(GetPawn()->GetActorForwardVector());
    }
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




