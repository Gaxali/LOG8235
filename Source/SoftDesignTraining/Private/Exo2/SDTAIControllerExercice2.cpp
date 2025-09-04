// Fill out your copyright notice in the Description page of Project Settings.

#include "Exo2/SDTAIControllerExercice2.h"
#include "SoftDesignTraining.h"
#include "Exo2/SDTProjectile.h"
#include "DrawDebugHelpers.h"
#include "Exo2/PawnDataContainerComponent.h"
#include "Components/SplineComponent.h"
#include "PhysicsHelpers.h"
#include "Engine/OverlapResult.h"

bool ASDTAIControllerExercice2::MoveToTarget(FVector2D target, float speed, float deltaTime)
{
	return ASDTAIController::MoveToTarget(target, speed, deltaTime);
}

void ASDTAIControllerExercice2::MoveTowardsDirection(FVector2D direction, float speed, float deltaTime)
{
	return ASDTAIController::MoveTowardsDirection(direction, speed, deltaTime);
}

bool ASDTAIControllerExercice2::ReachSpline(USplineComponent const& spline, float deltaTime)
{
	return ASDTAIController::ReachSpline(spline, deltaTime);
}

bool ASDTAIControllerExercice2::FollowSpline(USplineComponent const& spline, float deltaTime)
{
	return ASDTAIController::FollowSpline(spline, deltaTime);
}

bool ASDTAIControllerExercice2::GetAroundObstacle(float deltaTime)
{
	return ASDTAIController::GetAroundObstacle(deltaTime);
}

float ASDTAIControllerExercice2::GetDistanceToSpline(USplineComponent const& spline) const
{
	FVector2D const actorPosition(GetPawn()->GetActorLocation());
	FVector2D const splineStartPoint = FindSplineClosestPosition(spline);
	return (splineStartPoint - actorPosition).Size();
}

bool ASDTAIControllerExercice2::FollowSplineInDynamicObstacles(USplineComponent const& spline, float deltaTime)
{
	return ASDTAIController::FollowSplineInDynamicObstacles(spline, deltaTime);
}

USplineComponent* ASDTAIControllerExercice2::GetClosestSplineWithoutObstacle(TArray<USplineComponent*>& splines) const
{
	return ASDTAIController::GetClosestSplineWithoutObstacle(splines);
}

FVector2D ASDTAIControllerExercice2::FindSplineClosestPosition(USplineComponent const& spline) const
{
	FVector2D const actorPosition(GetPawn()->GetActorLocation());
	return FVector2D(spline.FindLocationClosestToWorldLocation(FVector(actorPosition, 0.f), ESplineCoordinateSpace::World));
}

TArray<FOverlapResult> ASDTAIControllerExercice2::CollectTargetActorsInFrontOfCharacter(APawn const* pawn, PhysicsHelpers& physicHelper) const
{
	TArray<FOverlapResult> outResults;
	physicHelper.SphereOverlap(pawn->GetActorLocation() + pawn->GetActorForwardVector() * 500.0f, 500.0f, outResults, true);
	return outResults;
}