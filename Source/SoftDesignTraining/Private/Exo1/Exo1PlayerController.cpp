// LOG8235 - August 2020

#include "Exo1/Exo1PlayerController.h"
#include "SoftDesignTraining.h"
#include <cmath>
#include "DrawDebugHelpers.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#include "TargetActor.h"
#include "Engine/OverlapResult.h"

int AExo1PlayerController::SURFACE_TYPE_WOOD = SurfaceType1;
int AExo1PlayerController::SURFACE_TYPE_METAL = SurfaceType2;
int AExo1PlayerController::SURFACE_TYPE_GLASS = SurfaceType3;

// Called every frame
void AExo1PlayerController::Tick(float DeltaTime)
{
    UWorld * World = GetWorld();
    PhysicsHelpers newPhysicHelper(World);
    FVector currentLocation = GetCharacter()->GetActorLocation();

	TArray<AActor*> resetActors;
	UGameplayStatics::GetAllActorsOfClass(World, ATargetActor::StaticClass(), resetActors);
	for (AActor* actor : resetActors)
	{
		((ATargetActor*)actor)->SetVisibilityInformation(false, false);
	}
	DrawCharacterAxes(World, GetPawn());
	DrawVisionCone(World, GetPawn(), GetPawn()->GetActorForwardVector(), m_visionAngle);
    TArray<FOverlapResult> foundActors = CollectTargetActorsInFrontOfCharacter(GetPawn(), newPhysicHelper);

	for (FOverlapResult overlapResult : foundActors)
    {
		if (ATargetActor* targetActor = dynamic_cast<ATargetActor*>(overlapResult.GetActor()))
		{
			if (IsInsideCone(GetPawn(), targetActor))
			{
				bool canSee = false;
				bool canShoot = false;
				GetVisibilityInformationForTarget(targetActor, currentLocation, newPhysicHelper, canSee, canShoot);
				targetActor->SetVisibilityInformation(canSee, canShoot);
			}
		}
    }
}

void AExo1PlayerController::GetVisibilityInformationForTarget(ATargetActor* targetActor, FVector selfPosition, PhysicsHelpers& physicHelper, bool& canSee, bool& canShoot)
{
}

void AExo1PlayerController::DrawCharacterAxes(UWorld* world, APawn* pawn) const
{
}

void AExo1PlayerController::DrawVisionCone(UWorld* world, APawn* pawn, FVector const& dir, float angle) const
{
}

TArray<FOverlapResult> AExo1PlayerController::CollectTargetActorsInFrontOfCharacter(APawn const* pawn, PhysicsHelpers& physicHelper) const
{
	TArray<FOverlapResult> outResults;
	return outResults;
}

bool AExo1PlayerController::IsInsideCone(APawn * pawn, ATargetActor * targetActor) const
{
	return false;
}
