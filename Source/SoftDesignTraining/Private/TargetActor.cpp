// Fill out your copyright notice in the Description page of Project Settings.

#include "TargetActor.h"
#include "SoftDesignTraining.h"


// Sets default values
ATargetActor::ATargetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATargetActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATargetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATargetActor::SetVisibilityInformation(bool isVisible, bool canShoot)
{
    UMeshComponent* meshComponent = FindComponentByClass<UMeshComponent>();

    if (isVisible && canShoot)
    {
        meshComponent->SetMaterial(0, VisibleMaterial);
    }
    else if (isVisible && !canShoot)
    {
        meshComponent->SetMaterial(0, VisibleNotShootableMaterial);
    }
    else if (!isVisible && canShoot)
    {
        meshComponent->SetMaterial(0, ShootableMaterial);
    }
    else
    {
        meshComponent->SetMaterial(0, NonVisibleMaterial);
    }
}

