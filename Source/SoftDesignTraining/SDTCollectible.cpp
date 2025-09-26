// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTCollectible.h"
#include "SoftDesignTraining.h"
#include "SoundFeedbackComponent.h"
#include "FXFeedbackComponent.h"


ASDTCollectible::ASDTCollectible()
{
    
    SoundFeedback = CreateDefaultSubobject<USoundFeedbackComponent>(TEXT("SoundFeedback"));
    FXFeedback = CreateDefaultSubobject<UFXFeedbackComponent>(TEXT("FXFeedback"));

    PrimaryActorTick.bCanEverTick = true;
}

void ASDTCollectible::BeginPlay()
{
    Super::BeginPlay();
}

void ASDTCollectible::Collect(AActor* Collector)
{
    GetWorld()->GetTimerManager().SetTimer(m_CollectCooldownTimer, this, &ASDTCollectible::OnCooldownDone, m_CollectCooldownDuration, false);

    GetStaticMeshComponent()->SetVisibility(false);

    // Déclenche le feedback (son ou FX choisi dans l’éditeur)
   
    if (SoundFeedback)
    {
        SoundFeedback->TriggerFeedback(Collector);
    }

    if (FXFeedback)
    {
        FXFeedback->TriggerFeedback(Collector);
    }
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

void ASDTCollectible::Tick(float deltaTime)
{
    Super::Tick(deltaTime);
}
