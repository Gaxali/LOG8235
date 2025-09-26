#pragma once

#include "CoreMinimal.h"
#include "SDTFeedbackComponent.h"
#include "FXFeedbackComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SOFTDESIGNTRAINING_API UFXFeedbackComponent : public USDTFeedbackComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
    UParticleSystem* FX;

    virtual void TriggerFeedback(AActor* InstigatorActor) override;
};