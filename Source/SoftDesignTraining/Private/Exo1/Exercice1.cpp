// LOG8235 - August 2020

#include "Exo1/Exercice1.h"
#include "SoftDesignTraining.h"
#include <cmath>
#include "DrawDebugHelpers.h"
#include "TargetActor.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Engine/OverlapResult.h"

void AExercice1::GetVisibilityInformationForTarget(ATargetActor* targetActor, FVector selfPosition, PhysicsHelpers& physicHelper, bool& canSee, bool& canShoot)
{
    TArray<struct FHitResult> hitResult;
    physicHelper.CastRay(selfPosition, targetActor->GetActorLocation(), hitResult, true);

    canSee = true;
    canShoot = true;
    for (int j = 0; j < hitResult.Num(); ++j)
    {
        if (hitResult[j].GetActor() != targetActor)
        {
            UPhysicalMaterial* PhysicsMtl = hitResult[j].PhysMaterial.Get();
            if (PhysicsMtl != nullptr)
            {
                if (PhysicsMtl->SurfaceType == SURFACE_TYPE_WOOD)
                {
                    canSee = false;
                }
                else if (PhysicsMtl->SurfaceType == SURFACE_TYPE_GLASS)
                {
                    canShoot = false;
                }
                else
                {
                    canSee = false;
                    canShoot = false;
                }

                if (canSee && canShoot)
                {
                    DrawDebugSphere(GetWorld(), hitResult[j].Location, 100.f, 32, FColor::Green);
                }
                else if (canShoot)
                {
                    DrawDebugSphere(GetWorld(), hitResult[j].Location, 100.f, 32, FColor::Yellow);
                }
                else if (canSee && !canShoot)
                {
                    DrawDebugSphere(GetWorld(), hitResult[j].Location, 100.f, 32, FColor::Red);
                }
                else
                {
                    DrawDebugSphere(GetWorld(), hitResult[j].Location, 100.f, 32, FColor::Black);
                }
            }
        }
    }
}

void AExercice1::DrawCharacterAxes(UWorld * world, APawn * pawn) const
{
	
	DrawDebugDirectionalArrow(world, pawn->GetActorLocation(), pawn->GetActorLocation() + pawn->GetActorForwardVector() * 100, 2, FColor::Red, false);
	DrawDebugDirectionalArrow(world, pawn->GetActorLocation(), pawn->GetActorLocation() + pawn->GetActorRightVector() * 100, 2, FColor::Blue, false);
	DrawDebugDirectionalArrow(world, pawn->GetActorLocation(), pawn->GetActorLocation() + pawn->GetActorUpVector() * 100, 2, FColor::Green, false);

}

void AExercice1::DrawVisionCone(UWorld * world, APawn * pawn, FVector const & dir, float angle) const
{
	DrawDebugSphere(world, pawn->GetActorLocation() + pawn->GetActorForwardVector() * 750, 1000, 32, FColor::Red);
}

TArray<FOverlapResult> AExercice1::CollectTargetActorsInFrontOfCharacter(APawn const * pawn, PhysicsHelpers & physicHelper) const
{
	TArray<FOverlapResult> v;
	physicHelper.SphereOverlap(pawn->GetActorLocation() + pawn->GetActorForwardVector() * 750, 1000, v, true);
	return v;
}

bool AExercice1::IsInsideCone(APawn * pawn, ATargetActor * targetActor) const
{
	auto v = targetActor->GetActorLocation() - pawn->GetActorLocation();
	v.Normalize();
	auto Angle = FMath::Acos(pawn->GetActorForwardVector().Dot(v)) * (180 / 3.141516);

	//UE_LOG(LogTemp, Warning, TEXT("MyValue is: %f"), Angle * (180 / 3.141516));

	return FMath::Abs(Angle) < 30;
}
