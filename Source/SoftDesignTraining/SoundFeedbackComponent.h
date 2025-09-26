#pragma once

#include "CoreMinimal.h"
#include "SDTFeedbackComponent.h"
#include "SoundFeedbackComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SOFTDESIGNTRAINING_API USoundFeedbackComponent : public USDTFeedbackComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
    USoundBase* Sound;

    virtual void TriggerFeedback(AActor* InstigatorActor) override;
};