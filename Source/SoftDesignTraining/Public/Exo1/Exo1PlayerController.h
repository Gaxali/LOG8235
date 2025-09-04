// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoftDesignTrainingPlayerController.h"
#include "PhysicsHelpers.h"
#include "TargetActor.h"
#include "PhysicsHelpers.h"

#include "Exo1PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API AExo1PlayerController : public ASoftDesignTrainingPlayerController
{
	GENERATED_BODY()

public:
    static int SURFACE_TYPE_WOOD;
    static int SURFACE_TYPE_METAL;
    static int SURFACE_TYPE_GLASS;

protected:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

	virtual void DrawCharacterAxes(UWorld* world, APawn* actor) const;
	virtual void DrawVisionCone(UWorld* world, APawn* pawn, FVector const& dir, float angle) const;
	virtual TArray<FOverlapResult> CollectTargetActorsInFrontOfCharacter(APawn const* pawn, PhysicsHelpers& physicHelper) const;
	virtual bool IsInsideCone(APawn * pawn, ATargetActor* targetActor) const;
    virtual void GetVisibilityInformationForTarget(ATargetActor* targetActor, FVector selfPosition, PhysicsHelpers& physicHelper, bool& canSee, bool& canShoot);

	float const m_visionAngle = PI / 3.0f;
};
