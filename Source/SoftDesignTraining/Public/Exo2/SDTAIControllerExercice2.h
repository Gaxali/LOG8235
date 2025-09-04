// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Exo2/SDTAIController.h"
#include "SDTAIControllerExercice2.generated.h"

class USplineComponent;
class PhysicsHelpers;

UCLASS()
class SOFTDESIGNTRAINING_API ASDTAIControllerExercice2 : public ASDTAIController
{
	GENERATED_BODY()
public:
	virtual bool MoveToTarget(FVector2D target, float speed, float deltaTime) override;
	virtual void MoveTowardsDirection(FVector2D direction, float speed, float deltaTime) override;
	virtual bool ReachSpline(USplineComponent const& spline, float deltaTime) override;
	virtual bool FollowSpline(USplineComponent const& spline, float deltaTime) override;
	virtual bool GetAroundObstacle(float deltaTime) override;
	virtual float GetDistanceToSpline(USplineComponent const& spline) const override;
	virtual bool FollowSplineInDynamicObstacles(USplineComponent const& spline, float deltaTime) override;
	virtual USplineComponent* GetClosestSplineWithoutObstacle(TArray<USplineComponent*>& splines) const override;
	virtual FVector2D FindSplineClosestPosition(USplineComponent const& spline) const override;
	virtual TArray<FOverlapResult> CollectTargetActorsInFrontOfCharacter(APawn const* pawn, PhysicsHelpers& physicHelper) const override;
};
