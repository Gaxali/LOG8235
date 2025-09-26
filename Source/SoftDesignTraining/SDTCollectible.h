// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "SDTCollectible.generated.h"


class USoundFeedbackComponent;
class UFXFeedbackComponent;
/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API ASDTCollectible : public AStaticMeshActor
{
	GENERATED_BODY()
public:
    ASDTCollectible();

    void Collect(AActor* Collector);
    void OnCooldownDone();
    bool IsOnCooldown();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_CollectCooldownDuration = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
        bool isMoveable = false;

    virtual void Tick(float deltaTime) override;
    virtual void BeginPlay() override;

    FVector initialPosition;

protected:
    FTimerHandle m_CollectCooldownTimer;

    // Feedback sélectionnable dans l’éditeur
    UPROPERTY(VisibleAnywhere, Instanced, Category = "Feedback")
    USoundFeedbackComponent* SoundFeedback;

    UPROPERTY(VisibleAnywhere, Instanced, Category = "Feedback")
    UFXFeedbackComponent* FXFeedback;
	
};
