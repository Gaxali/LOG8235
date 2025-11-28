#include "BTTask_MovePatrol.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"

EBTNodeResult::Type UBTTask_MovePatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* aiController = OwnerComp.GetAIOwner();
    if (!aiController)
        return EBTNodeResult::Failed;

    APawn* pawn = aiController->GetPawn();
    if (!pawn)
        return EBTNodeResult::Failed;

    UWorld* world = pawn->GetWorld();
    if (!world)
        return EBTNodeResult::Failed;

    const FVector Forward = pawn->GetActorForwardVector();
    const FVector Start = pawn->GetActorLocation();
    const float DetectDistance = 300.0f;
    const FVector End = Start + Forward * DetectDistance;

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(pawn);

    bool bHit = world->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_WorldStatic, Params);

    FVector MoveDirection = Forward;
    if (bHit)
    {
        MoveDirection = pawn->GetActorRightVector();
    }

    if (!MoveDirection.IsNearlyZero())
        MoveDirection = MoveDirection.GetSafeNormal();

    if (ACharacter* Character = Cast<ACharacter>(pawn))
    {
        if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
        {
            MoveComp->AddInputVector(MoveDirection);
        }
    }

    DrawDebugLine(world, Start, End, bHit ? FColor::Red : FColor::Green, false, 1.0f, 0, 2.0f);

    return EBTNodeResult::Succeeded;
}