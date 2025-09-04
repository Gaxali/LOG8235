// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Exo1/Exo1PlayerController.h"
#include "Exercice1.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API AExercice1 : public AExo1PlayerController
{
	GENERATED_BODY()

protected:
    virtual void GetVisibilityInformationForTarget(ATargetActor* targetActor, FVector selfPosition, PhysicsHelpers& physicHelper, bool& canSee, bool& canShoot);
	virtual void DrawCharacterAxes(UWorld* world, APawn* pawn) const;
	virtual void DrawVisionCone(UWorld* world, APawn* pawn, FVector const& dir, float angle) const;
	virtual TArray<FOverlapResult> CollectTargetActorsInFrontOfCharacter(APawn const* pawn, PhysicsHelpers& physicHelper) const;
	virtual bool IsInsideCone(APawn * pawn, ATargetActor* targetActor) const;

	float const m_maxVisionAngle = PI / 3.0f;
};
