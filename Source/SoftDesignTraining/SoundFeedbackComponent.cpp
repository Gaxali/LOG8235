#include "SoundFeedbackComponent.h"
#include "Kismet/GameplayStatics.h"

void USoundFeedbackComponent::TriggerFeedback(AActor* InstigatorActor)
{
    if (Sound && InstigatorActor)
    {
        UGameplayStatics::PlaySoundAtLocation(
            InstigatorActor->GetWorld(),
            Sound,
            InstigatorActor->GetActorLocation()
        );
    }
}