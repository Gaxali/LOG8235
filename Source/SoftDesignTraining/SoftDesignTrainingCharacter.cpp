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

void ASoftDesignTrainingCharacter::MoveForward(float Value, float DeltaTime)
{
    //FRotator ControlRot(GetControlRotation());
    //ControlRot.Pitch = 0.0f;
    //ControlRot.Roll = 0.0f;

    AddMovementInput(GetActorForwardVector(), Value);

 
    //auto DirVelocity = GetMovementComponent()->Velocity.GetSafeNormal();

    //AddMovementInput(-DirVelocity, 0.5 * Value);
    
    //float Acceleration = 20;
    //
    //auto DirVelocity = GetMovementComponent()->Velocity.GetSafeNormal();
    //GetMovementComponent()->Velocity = FVector::Zero();// += Acceleration * DirVelocity * DeltaTime;
    //
    //auto V = GetMovementComponent()->Velocity;
    //
    //UE_LOG(LogTemp, Warning, TEXT("MyValue is: %f , %f, %f"), V.X, V.Y, V.Z);

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

void ASoftDesignTrainingCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    MoveForward(1, DeltaTime);
}