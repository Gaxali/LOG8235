// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SoftDesignTrainingCharacter.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTrainingMainCharacter.h"
#include "SDTUtils.h"
#include "DrawDebugHelpers.h"
#include "SDTCollectible.h"


ASoftDesignTrainingCharacter::ASoftDesignTrainingCharacter()
{
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    PrimaryActorTick.bCanEverTick = true;
}

void ASoftDesignTrainingCharacter::BeginPlay()
{
    Super::BeginPlay();

    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ASoftDesignTrainingCharacter::OnBeginOverlap);
    m_StartingPosition = GetActorLocation();
}

void ASoftDesignTrainingCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherComponent->GetCollisionObjectType() == COLLISION_DEATH_OBJECT)
    {
        SetActorLocation(m_StartingPosition);
    }
    else if(ASDTCollectible* collectibleActor = Cast<ASDTCollectible>(OtherActor))
    {
        if (!collectibleActor->IsOnCooldown())
        {
            OnCollectPowerUp();
        }

        collectibleActor->Collect();
    }
    else if (ASoftDesignTrainingMainCharacter* mainCharacter = Cast<ASoftDesignTrainingMainCharacter>(OtherActor))
    {
        if(mainCharacter->IsPoweredUp())
            SetActorLocation(m_StartingPosition);
    }
}


void ASoftDesignTrainingCharacter::ApplyAcceleration(const FVector& Acceleration, float DeltaTime, float MaxSpeed)
{
    //Calculer la nouvelle vitesse avec l'accélération
    CurrentVelocity += Acceleration * DeltaTime;

    //Limitation de vitesse avec MaxSpeed
    if (CurrentVelocity.Size() > MaxSpeed)
    {
        CurrentVelocity = CurrentVelocity.GetSafeNormal() * MaxSpeed;
    }

    //Déplacement de l'agent
    FVector NewLocation = GetActorLocation() + CurrentVelocity * DeltaTime;
    SetActorLocation(NewLocation, false);

    //Orienter l'agent dans la direction de la vitesse
    if (!CurrentVelocity.IsNearlyZero())
    {
        //Ignorer la vitesse verticale pour la rotation
        FVector HorizontalVelocity = FVector(CurrentVelocity.X, CurrentVelocity.Y, 0.0f);

        if (!HorizontalVelocity.IsNearlyZero())
        {
            FRotator NewRotation = HorizontalVelocity.ToOrientationRotator();
            SetActorRotation(NewRotation);
        }
    }
}


void ASoftDesignTrainingCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);   
}