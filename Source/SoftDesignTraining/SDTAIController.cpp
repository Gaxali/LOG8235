// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"

#include "SDTStateMachine.h"
#include "Engine/CollisionProfile.h"

#include "SoftDesignTrainingMainCharacter.h"


ASDTAIController::ASDTAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    StateMachine = CreateDefaultSubobject<USDTStateMachine>("AIStateMachine");

    // Add walls and obstacles
    QueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

    FCollisionResponseTemplate Response;
    if (UCollisionProfile::Get()->GetProfileTemplate(ChannelName, Response))
    {
        QueryParams.AddObjectTypesToQuery(Response.ObjectType);
    }
    //
}

void ASDTAIController::BeginPlay()
{
    Super::BeginPlay();

    auto AIPawn = GetPawn();

    if (AIPawn)
    {
        CharacterAI = Cast<ACharacter>(AIPawn);
    }
}

bool ASDTAIController::TestRaycast(float AngleSideDegrees, float AngleDegreesDown, float Length, FHitResult& ResultOut, FColor Color)
{
    bool Success = false;
     
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

    //DrawDebugDirectionalArrow(GetWorld(), PositionStart, PositionEnd, 2, Color, false);

    return Success;
}

void ASDTAIController::AddMovement(FVector NewDirection)
{

    //GetPawn()->AddMovementInput(NewDirection, 1);
    //GetPawn()->SetActorRotation(NewDirection.ToOrientationQuat());

    if (auto* MyCharacter = Cast<ASoftDesignTrainingCharacter>(GetPawn()))
    {
        FVector Accel = NewDirection * 2500.0f; //forte accélération pour virage rapide
        MyCharacter->ApplyAcceleration(Accel, GetWorld()->GetDeltaSeconds(), MaxWalkSpeed);
    }
}

void ASDTAIController::AddMovementSides(const FHitResult& ResultIn, float Sign)
{
    int Factor = FMath::RandRange(0, 20) / 100;

    auto TangenDirection = Sign * FVector::CrossProduct(GetPawn()->GetActorUpVector(), ResultIn.ImpactNormal);
    auto NewDirection = (TangenDirection + ResultIn.ImpactNormal * Factor).GetSafeNormal();
    AddMovement(NewDirection);
}

bool ASDTAIController::HitObjectByChannelName(FHitResult& Result)
{
    const UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(Result.GetComponent());
    if (!Component) return false;

    FCollisionResponseTemplate Response;
    if (UCollisionProfile::Get()->GetProfileTemplate(ChannelName, Response))
    {
        return Component->GetCollisionObjectType() == Response.ObjectType;
    }
        
    return false;
}

void ASDTAIController::AvoidingObstaces(const FVector& Direction)
{
    FHitResult ResultLeftRay;
    FHitResult ResultRightRay;
    
    bool CollisionDetectionLeftRay = TestRaycast(-AngleSideDegree, AngleDownDegree, 200, ResultLeftRay);
    bool CollisionDetectionRightRay = TestRaycast(AngleSideDegree, AngleDownDegree, 200, ResultRightRay);

    bool ResultLeftRayHitDeathFloor = HitObjectByChannelName(ResultLeftRay);
    bool ResultRightRayHitDeathFloor = HitObjectByChannelName(ResultRightRay);

    if (CollisionDetectionLeftRay && CollisionDetectionRightRay)
    {
        FVector TotalDirection = (ResultLeftRay.ImpactNormal + ResultRightRay.ImpactNormal).GetSafeNormal();

        if (ResultLeftRayHitDeathFloor || ResultRightRayHitDeathFloor)
        {
            TotalDirection = -GetPawn()->GetActorForwardVector();
        }

        int AngleDeg = FMath::RandRange(-10, 10);
        FVector Up(0, 0, 1);
        FVector VectorRotated = TotalDirection.RotateAngleAxis(AngleDeg, Up);

        AddMovement(VectorRotated);
    }
    else if (CollisionDetectionLeftRay)
    {
        if (ResultLeftRayHitDeathFloor)
        {
            AddMovement(-GetPawn()->GetActorForwardVector());
        }
        else
        {
            AddMovementSides(ResultLeftRay, -1);
        }
        
    }
    else if (CollisionDetectionRightRay)
    {
        if (ResultRightRayHitDeathFloor)
        {
            AddMovement(-GetPawn()->GetActorForwardVector());
        }
        else
        {
            AddMovementSides(ResultRightRay, 1);
        }
    }
    else
    {
        AddMovement(Direction);
    }
}


void ASDTAIController::Navigation(const FVector& Direction)
{
    AvoidingObstaces(Direction);

}

void ASDTAIController::Tick(float deltaTime)
{ 
    Super::Tick(deltaTime);

    if (StateMachine)
    {
        FVector OutDirection;
        StateMachine->Run(GetPawn(), OutDirection);

        Navigation(OutDirection);
    }
}




