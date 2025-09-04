// Fill out your copyright notice in the Description page of Project Settings.

#include "Exo2/SDTAIController.h"
#include "SoftDesignTraining.h"
#include "Exo2/SDTProjectile.h"
#include "DrawDebugHelpers.h"
#include "Exo2/PawnDataContainerComponent.h"
#include "Components/SplineComponent.h"
#include "PhysicsHelpers.h"
#include <cmath>
#include "Engine/OverlapResult.h"

ASDTAIController::ASDTAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ASDTAIController::BeginPlay()
{
	Super::BeginPlay();
	m_capsuleRadius = 0.0f;

	if (UCapsuleComponent const* capsule = GetPawn()->FindComponentByClass<UCapsuleComponent>())
	{
		m_capsuleRadius = capsule->GetScaledCapsuleRadius() + 4.0f;
	}

	m_StartingPosition = FVector2D(GetPawn()->GetActorLocation());
}

void ASDTAIController::Tick(float deltaTime)
{
	if (!m_chosenSpline)
	{
		TArray<USplineComponent*> splines;
		for (AActor const* actor : GetPawn()->FindComponentByClass<UPawnDataContainerComponent>()->m_spline)
		{
			if (USplineComponent* spline = actor->FindComponentByClass<USplineComponent>())
			{
				splines.Push(spline);
			}
		}

		if (USplineComponent* spline = GetClosestSplineWithoutObstacle(splines))
		{
			m_chosenSpline = spline;
		}
	}

	if (m_chosenSpline)
	{
		InternalTick(*m_chosenSpline, deltaTime);
	}
}

void ASDTAIController::InternalTick(USplineComponent const& spline, float deltaTime)
{
	switch (m_stage)
	{
		case Stage::ReachSpline:
		{
			if (ReachSpline(spline, deltaTime))
			{
				m_stage = Stage::FollowSpline;
			}

			if (m_hitInformation.HandleHit())
			{
				m_stage = Stage::GetAroundObstacle;
			}
		}
		break;
		case Stage::FollowSpline:
		{
			if (FollowSpline(spline, deltaTime))
			{
				m_stage = Stage::DynamicObstacle;
			}

			if (GetDistanceToSpline(spline) > 100.0f)
			{
				m_stage = Stage::ReachSpline;
			}

			if (m_hitInformation.HandleHit())
			{
				m_stage = Stage::GetAroundObstacle;
			}

			float newSpeed = m_maxSpeed;
			FVector2D newDirection = FVector2D(spline.FindDirectionClosestToWorldLocation(GetPawn()->GetActorLocation(), ESplineCoordinateSpace::World));
			if (RecoverIncomingCollisions(newDirection.GetSafeNormal() * newSpeed).Num() != 0)
			{
				m_stage = Stage::DynamicObstacle;
			}
		}
		break;
		case Stage::GetAroundObstacle:
		{
			if (FollowSpline(spline, deltaTime))
			{
				m_stage = Stage::DynamicObstacle;
			}

			if (GetAroundObstacle(deltaTime))
			{
				m_stage = Stage::FollowSpline;
			}
		}
		break;
		case Stage::DynamicObstacle:
		{
			if (FollowSplineInDynamicObstacles(spline, deltaTime))
			{
				m_stage = Stage::Idle;
			}
		}
		break;
		case Stage::Idle:
		{

		}
		break;
	};
}

TArray<CollisionInfo> ASDTAIController::RecoverIncomingCollisions(FVector2D projectedVelocity) const
{
	TArray<CollisionInfo> collisions;
    UWorld * World = GetWorld();
    PhysicsHelpers physicHelper(World);
	TArray<FOverlapResult> overlapResults = CollectTargetActorsInFrontOfCharacter(GetPawn(), physicHelper);

    for (FOverlapResult overlapResult : overlapResults)
    {
		if (ASDTProjectile* projectile = dynamic_cast<ASDTProjectile*>(overlapResult.GetActor()))
		{
			float projectileRadius;
			FVector projectilePosition;
			projectile->GetPositionAndRadius(projectileRadius, projectilePosition);

			float const squaredRadiusSum = (m_capsuleRadius + projectileRadius) * (m_capsuleRadius + projectileRadius);

			FVector2D const initialProjectilePosition = FVector2D(projectile->GetActorLocation());
			FVector2D const projectileVelocity = projectile->GetProjectileVelocity();
			FVector2D const initialPositionDifference = FVector2D(GetPawn()->GetActorLocation()) - initialProjectilePosition;
			FVector2D const relativeVelocity = projectedVelocity - projectileVelocity;
			float const Dix = initialPositionDifference.X;
			float const Diy = initialPositionDifference.Y;
			float const Vrx = relativeVelocity.X;
			float const Vry = relativeVelocity.Y;

			// solve for at^2 + bt + c = 0
			float const a = Vrx * Vrx + Vry * Vry;
			float const b = 2 * Vrx * Dix + 2 * Vry * Diy;
			float const c = Dix * Dix + Diy * Diy - squaredRadiusSum;

			if (b * b - 4 * a * c < 0)
			{
				DrawDebugString(GetWorld(), FVector(initialProjectilePosition.X, initialProjectilePosition.Y, 200.0f), FString("No collision incoming"), nullptr, FColor::White, -0.0f, false);
				continue;
			}

			// float const tf = (-b + sqrt(b * b - 4 * a * c)) / (2 * a); final time of collision
			float const ti = (-b - sqrt(b * b - 4 * a * c)) / (2 * a); // Start time of collision

			if (ti < 0.0f)
			{
				DrawDebugString(GetWorld(), FVector(initialProjectilePosition.X, initialProjectilePosition.Y, 200.0f), FString("No collision incoming"), nullptr, FColor::White, -0.0f, false);
				continue;
			}

			// Compute collision point
			FVector2D const projectileCollisionPosition = initialProjectilePosition + ti * projectileVelocity;
			FVector2D const pawnCollisionPosition = FVector2D(GetPawn()->GetActorLocation()) + ti * projectedVelocity;
			FVector2D const collisionPoint = pawnCollisionPosition + (projectileCollisionPosition - pawnCollisionPosition).GetSafeNormal() * m_capsuleRadius;
			collisions.Push(CollisionInfo(ti, collisionPoint));
			DrawDebugString(GetWorld(), FVector(collisionPoint.X, collisionPoint.Y, 200.0f), FString("Time to collision is : ") + FString::SanitizeFloat(ti), nullptr, FColor::Red, -0.0f, false);
		}
	}

	return collisions;
}

FVector2D ASDTAIController::FindSplineClosestPosition(USplineComponent const& spline) const
{
	return FVector2D();
}

TArray<FOverlapResult> ASDTAIController::CollectTargetActorsInFrontOfCharacter(APawn const * pawn, PhysicsHelpers & physicHelper) const
{
	TArray<FOverlapResult> outResults;
	return outResults;
}

void ASDTAIController::NotifyHit(UPrimitiveComponent * MyComp, AActor * Other, UPrimitiveComponent * OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit)
{
	m_hitInformation.ReceivedHit(FVector2D(HitNormal));
}

bool ASDTAIController::ReachSpline(USplineComponent const& spline, float deltaTime)
{
	return false;
}

bool ASDTAIController::FollowSpline(USplineComponent const& spline, float deltaTime)
{
	return false;
}

bool ASDTAIController::MoveToTarget(FVector2D target, float targetSpeed, float deltaTime)
{
	return false;
}

void ASDTAIController::MoveTowardsDirection(FVector2D direction, float targetSpeed, float deltaTime)
{
}

bool ASDTAIController::GetAroundObstacle(float deltaTime)
{
	return false;
}

float ASDTAIController::GetDistanceToSpline(USplineComponent const& spline) const
{
	return 0.f;
}

bool ASDTAIController::FollowSplineInDynamicObstacles(USplineComponent const& spline, float deltaTime)
{
	return false;
}

USplineComponent * ASDTAIController::GetClosestSplineWithoutObstacle(TArray<USplineComponent*>& splines) const
{
	return nullptr;
}
