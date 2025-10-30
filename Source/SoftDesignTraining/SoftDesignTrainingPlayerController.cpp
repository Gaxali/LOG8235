// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SoftDesignTrainingPlayerController.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTrainingMainCharacter.h"

#include "DrawDebugHelpers.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "SDTBridge.h"
#include "SDTBoatOperator.h"
#include "Engine/OverlapResult.h"
#include "NavigationSystem.h" 

ASoftDesignTrainingPlayerController::ASoftDesignTrainingPlayerController()
{
    // Make a path following component
    m_PathFollowingComponent = CreateDefaultSubobject<USDTPathFollowingComponent>(TEXT("PathFollowingComponent"));
}

void ASoftDesignTrainingPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Move camera
    InputComponent->BindAxis("MoveForward", this, &ASoftDesignTrainingPlayerController::MoveCameraForward);
    InputComponent->BindAxis("MoveRight", this, &ASoftDesignTrainingPlayerController::MoveCameraRight);

    // Zoom camera
    InputComponent->BindAxis("Zoom", this, &ASoftDesignTrainingPlayerController::ZoomCamera);

    // Move character on click
    InputComponent->BindAction("MoveCharacter", EInputEvent::IE_Released, this, &ASoftDesignTrainingPlayerController::MoveCharacter);
    
    InputComponent->BindAction("Activate", EInputEvent::IE_Pressed, this, &ASoftDesignTrainingPlayerController::Activate);
    InputComponent->BindAction("Activate", EInputEvent::IE_Released, this, &ASoftDesignTrainingPlayerController::Deactivate);
}

void ASoftDesignTrainingPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Always show the mouse
    bShowMouseCursor = true;

    // Allow character to be moved by default 
    m_CanMoveCharacter = true;

    // In case we are activating a bridge
    m_BridgeActivated = nullptr;

    // In case we are activating a boat operator
    m_BoatOperatorActivated = nullptr;
}

void ASoftDesignTrainingPlayerController::MoveCameraForward(float value)
{
    ASoftDesignTrainingMainCharacter* character = Cast<ASoftDesignTrainingMainCharacter>(GetPawn());

    if (character)
    {
        character->MoveCameraForward(value);
    }
}

void ASoftDesignTrainingPlayerController::MoveCameraRight(float value)
{
    ASoftDesignTrainingMainCharacter* character = Cast<ASoftDesignTrainingMainCharacter>(GetPawn());

    if (character)
    {
        character->MoveCameraRight(value);
    }
}

void ASoftDesignTrainingPlayerController::ZoomCamera(float axisValue)
{
    ASoftDesignTrainingMainCharacter* character = Cast<ASoftDesignTrainingMainCharacter>(GetPawn());

    if (character)
    {
        character->ZoomCamera(axisValue);
    }
}

#include "NavigationPath.h"

void ASoftDesignTrainingPlayerController::DebugDrawCalculatedPath(UNavigationPath* NavPath)
{
    UE_LOG(LogTemp, Display, TEXT("Path found with %d points."), NavPath->PathPoints.Num());

    if (NavPath->PathPoints.IsEmpty())
        return;

    // Visualize it
    for (int32 i = 0; i < NavPath->PathPoints.Num() - 1; ++i)
    {
        //DrawDebugSphere(GetWorld(), NavPath->PathPoints[i], 25.f, 8, FColor::Green, false, 3.f);
        FVector StartSegment = NavPath->PathPoints[i];
        FVector EndSegment = NavPath->PathPoints[i + 1];

        DrawDebugLine(
            GetWorld(),
            StartSegment,
            EndSegment,
            FColor::Green,
            false,   // persistent lines? (false means they disappear after time)
            5.0f,    // duration in seconds
            0,
            6.0f     // thickness
        );

        // Draw small spheres at the path points
        DrawDebugSphere(GetWorld(), StartSegment, 20.f, 12, FColor::Yellow, false, 5.0f);
    }

    // Draw the last point
    DrawDebugSphere(GetWorld(), NavPath->PathPoints.Last(), 25.f, 12, FColor::Red, false, 5.0f);
}

void ASoftDesignTrainingPlayerController::MoveCharacter()
{
    // TODO : find the position of the mouse in the world 
    // And move the agent to this position IF possible
    // Validate you can move through m_CanMoveCharacter
    
    FVector WorldLocation;
    FVector WorldDirection;

    DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

    FHitResult ResultOut;
    FCollisionObjectQueryParams QueryParams;
    QueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

    FVector StartLocation = WorldLocation;
    FVector EndLocation = WorldLocation + 10000.f * WorldDirection;

    bool Success = GetWorld()->LineTraceSingleByObjectType(ResultOut,
        StartLocation,
        EndLocation,
        QueryParams);

    if (Success)
    {
        if (m_CanMoveCharacter)
        {
            UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
            
            if (NavSys)
            {
                FNavLocation Projected;

                if (NavSys->ProjectPointToNavigation(ResultOut.ImpactPoint, Projected))
                {
                    ACharacter* MyChar = Cast<ACharacter>(GetPawn());
                    if (MyChar)
                    {
                        UNavigationPath* NavPath = NavSys->FindPathToLocationSynchronously(
                            GetWorld(),
                            MyChar->GetActorLocation(),
                            Projected.Location,
                            MyChar
                        );

                        if (NavPath)
                        {
                            DebugDrawCalculatedPath(NavPath);
                            UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Projected.Location);
                        }
                                                
                        //DrawDebugDirectionalArrow(GetWorld(), StartLocation, EndLocation, 2, FColor::Red, false, 3.f);
                        //DrawDebugSphere(GetWorld(), ResultOut.ImpactPoint, 10, 16, FColor::Green, false, 3, 0, 2);
                    }
                }
            }
        }
    }
}

void ASoftDesignTrainingPlayerController::Activate()
{
    APawn* pawn = GetPawn();
    if (pawn == nullptr)
    {
        return;
    }

    m_CanMoveCharacter = false;
    // TODO : Mouvement of the agent should be stopped !!

    // Make an overlap to find what is near us to activate it
    TArray<FOverlapResult> results;
    GetWorld()->OverlapMultiByChannel(results, pawn->GetActorLocation(), pawn->GetActorRotation().Quaternion(), ECollisionChannel::ECC_WorldDynamic, FCollisionShape::MakeSphere(200.f));

    for (FOverlapResult& result : results)
    {
        AActor* actor = result.GetActor();
        ASDTBridge* bridge = Cast<ASDTBridge>(actor);
        if (bridge)
        {
            bridge->Activate();
            m_BridgeActivated = bridge;
            break;
        }

        ASDTBoatOperator* boatOperator = Cast<ASDTBoatOperator>(actor);
        if (boatOperator)
        {
            boatOperator->Activate();
            m_BoatOperatorActivated = boatOperator;
            break;
        }
    }
}

void ASoftDesignTrainingPlayerController::Deactivate()
{
    m_CanMoveCharacter = true;

    if (m_BridgeActivated)
    {
        m_BridgeActivated->Deactivate();
    }
    if (m_BoatOperatorActivated)
    {
        m_BoatOperatorActivated->Deactivate();
    }
}