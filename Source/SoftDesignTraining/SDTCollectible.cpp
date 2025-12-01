// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTCollectible.h"
#include "SoftDesignTraining.h"
#include "SoftDesignTrainingMainCharacter.h"
#include "SoftDesignTrainingGameMode.h"

ASDTCollectible::ASDTCollectible()
{

}

void ASDTCollectible::Collect(ASoftDesignTrainingCharacter* InstigatorCharacter)
{
    if (ASoftDesignTrainingMainCharacter* Player = Cast<ASoftDesignTrainingMainCharacter>(InstigatorCharacter))
    {
        ASoftDesignTrainingGameMode* GM = GetWorld()->GetAuthGameMode<ASoftDesignTrainingGameMode>();
        if (GM)
        {
            GM->PlayerPickUpPowerUp();
        }
    }

    GetWorld()->GetTimerManager().SetTimer(m_CollectCooldownTimer, this, &ASDTCollectible::OnCooldownDone, m_CollectCooldownDuration, false);

    GetStaticMeshComponent()->SetVisibility(false);
}

void ASDTCollectible::OnCooldownDone()
{
    GetWorld()->GetTimerManager().ClearTimer(m_CollectCooldownTimer);

    GetStaticMeshComponent()->SetVisibility(true);
}

bool ASDTCollectible::IsOnCooldown()
{
    return m_CollectCooldownTimer.IsValid();
}
