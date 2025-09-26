#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SDTFeedbackComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SOFTDESIGNTRAINING_API USDTFeedbackComponent : public UActorComponent
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "Feedback")
    virtual void TriggerFeedback(AActor* InstigatorActor) PURE_VIRTUAL(UFeedbackComponent::TriggerFeedback, );
};
