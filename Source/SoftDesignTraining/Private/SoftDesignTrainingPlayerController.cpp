// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SoftDesignTrainingPlayerController.h"
#include "SoftDesignTraining.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "SoftDesignTrainingCharacter.h"
#include "ObjectPartition.h"

#include "DesignTrainingMovementComponent.h"
#include "CoverDetectionComponent.h"

#include "DrawDebugHelpers.h"

ASoftDesignTrainingPlayerController::ASoftDesignTrainingPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ASoftDesignTrainingPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}

    if (bZoomingIn)
    {
        bZoomingIn = false;
        APawn* const pawn = GetPawn();
        if (pawn)
        {
            ASoftDesignTrainingCharacter* sdtChar = Cast<ASoftDesignTrainingCharacter>(pawn);
            if (sdtChar)
            {
                if (sdtChar->GetCameraBoom()->TargetArmLength > 100)
                {
                    sdtChar->GetCameraBoom()->TargetArmLength -= 100;
                }
            }
        }
    }

    if (bZoomingOut)
    {
        bZoomingOut = false;
        APawn* const pawn = GetPawn();
        if (pawn)
        {
            ASoftDesignTrainingCharacter* sdtChar = Cast<ASoftDesignTrainingCharacter>(pawn);
            if (sdtChar)
            {
                if (sdtChar->GetCameraBoom()->TargetArmLength < 5000)
                {
                    sdtChar->GetCameraBoom()->TargetArmLength += 100;
                }
            }
        }
    }

    // hack until we know how to tick singleton
    ObjectPartition* op = ObjectPartition::GetInstance();
    if (op)
    { 
        op->Tick();
    }
}

void ASoftDesignTrainingPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ASoftDesignTrainingPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ASoftDesignTrainingPlayerController::OnSetDestinationReleased);
    InputComponent->BindAction("TakeCover", IE_Pressed, this, &ASoftDesignTrainingPlayerController::OnTakeCoverPressed);

	InputComponent->BindAction("PlaceBomb", IE_Pressed, this, &ASoftDesignTrainingPlayerController::OnPlaceBombPressed);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ASoftDesignTrainingPlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ASoftDesignTrainingPlayerController::MoveToTouchLocation);

    InputComponent->BindAxis("MoveForward", this, &ASoftDesignTrainingPlayerController::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &ASoftDesignTrainingPlayerController::MoveRight);

    //Hook up events for "ZoomIn"
    InputComponent->BindAction("ZoomIn", IE_Pressed, this, &ASoftDesignTrainingPlayerController::ZoomIn);
    InputComponent->BindAction("ZoomOut", IE_Pressed, this, &ASoftDesignTrainingPlayerController::ZoomOut);
}

void ASoftDesignTrainingPlayerController::MoveToMouseCursor()
{
	// Trace to see what is under the mouse cursor
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(Hit.ImpactPoint);
	}
}

void ASoftDesignTrainingPlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void ASoftDesignTrainingPlayerController::SetNewMoveDestination(const FVector& DestLocation)
{
	APawn* const pawn = GetPawn();
	if (pawn)
	{
		float const Distance = FVector::Dist(DestLocation, pawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (Distance > 120.0f)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void ASoftDesignTrainingPlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void ASoftDesignTrainingPlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}

void ASoftDesignTrainingPlayerController::MoveForward(float value)
{
    APawn* const pawn = GetPawn();

    if (pawn)
    {
        pawn->AddMovementInput(FVector(value, 0.0f, 0.0f));
    }
}

void ASoftDesignTrainingPlayerController::MoveRight(float value)
{
    APawn* const pawn = GetPawn();

    if (pawn)
    {
        pawn->AddMovementInput(FVector(0.0f, value, 0.0f));
    }
}

void ASoftDesignTrainingPlayerController::OnTakeCoverPressed()
{
    UCoverDetectionComponent* coverComponent = NULL;
    APawn* const pawn = GetPawn();

    if (pawn)
    {
        coverComponent = pawn->FindComponentByClass<UCoverDetectionComponent>();
    }
    
    if (coverComponent != NULL)
    {
        FVector coverLocation = FVector::ZeroVector;
        bool coverFound = coverComponent->DetectCoverNow(coverLocation, true);
        if (coverFound)
        {
            MoveToCoverDestination(coverLocation);
        }
    }
}

void ASoftDesignTrainingPlayerController::MoveToCoverDestination(const FVector& DestLocation)
{
    APawn* const pawn = GetPawn();
    if (pawn)
    {
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
    }
}
void ASoftDesignTrainingPlayerController::OnPlaceBombPressed()
{
	APawn* const pawn = GetPawn();
	if (pawn)
	{
		ASoftDesignTrainingCharacter* sdtChar = Cast<ASoftDesignTrainingCharacter>(pawn);
		if (sdtChar )
		{
			sdtChar->PlaceBomb();
		}
	}
}


void ASoftDesignTrainingPlayerController::ZoomIn()
{
    bZoomingIn = true;
}

void ASoftDesignTrainingPlayerController::ZoomOut()
{
    bZoomingOut = true;
}