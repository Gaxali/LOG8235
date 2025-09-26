#include "FXFeedbackComponent.h"
#include "Kismet/GameplayStatics.h"

void UFXFeedbackComponent::TriggerFeedback(AActor* InstigatorActor)
{
    if (FX && InstigatorActor)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            InstigatorActor->GetWorld(),
            FX,
            InstigatorActor->GetActorLocation(),
            FRotator::ZeroRotator,
            true
        );
    }
}