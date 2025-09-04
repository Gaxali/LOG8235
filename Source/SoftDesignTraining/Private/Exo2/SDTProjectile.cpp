// Fill out your copyright notice in the Description page of Project Settings.

#include "Exo2/SDTProjectile.h"
#include "SoftDesignTraining.h"

ASDTProjectile::ASDTProjectile()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
	SetMobility(EComponentMobility::Movable);
}

void ASDTProjectile::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
    if (m_Fired)
    {
        FVector impulse = m_Direction * m_Speed * deltaTime;
        SetActorLocation(GetActorLocation() + impulse);
    }
}

void ASDTProjectile::FireProjectile(const FVector& direction, float speed)
{
    m_Direction = direction;
    m_Speed = speed;

    m_Fired = true;
    m_StartingPosition = GetActorLocation();
}

void ASDTProjectile::ResetProjectile()
{
    SetActorLocation(m_StartingPosition);
}

void ASDTProjectile::GetPositionAndRadius(float& radius, FVector& pos)
{
	FVector extent;
	GetActorBounds(true, pos, extent);
	radius = extent[0];
}

FVector2D ASDTProjectile::GetInitialPosition() const
{
	return FVector2D(m_StartingPosition.X, m_StartingPosition.Y);
}

FVector2D ASDTProjectile::GetProjectileVelocity() const
{
	FVector velocity = m_Direction * m_Speed;
	return FVector2D(velocity.X, velocity.Y);
}
