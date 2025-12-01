// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SDTBaseAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "SDTAIController.generated.h"

/**
 * 
 */
class UBehaviorTreeComponent;
class UBlackboardComponent;
class UBehaviorTree;

UCLASS(ClassGroup = AI, config = Game)
class SOFTDESIGNTRAINING_API ASDTAIController : public ASDTBaseAIController
{
	GENERATED_BODY()

protected:
    virtual void OnPossess(APawn* pawn) override;

public:
    ASDTAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void BeginPlay() override;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_DetectionCapsuleHalfLength = 500.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_DetectionCapsuleRadius = 250.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float m_DetectionCapsuleForwardStartingOffset = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    UCurveFloat* JumpCurve;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float JumpApexHeight = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
    float JumpSpeed = 1.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    bool AtJumpSegment = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    bool InAir = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
    bool Landing = false;

public:
    uint16  GetTargetPosBBKeyID() const { return m_targetPosBBKeyID; }
    uint16  GetTargetSeenKeyID() const { return m_isTargetSeenBBKeyID; }
    uint16  GetNextPatrolDestinationKeyID() const { return m_nextPatrolDestinationBBKeyID; }
    uint16  GetCurrentPatrolDestinationKeyID() const { return m_currentPatrolDestinationBBKeyID; }
    uint16  GetIsSelectedForTacticalGroupID() const { return m_isSelectedForTacticalGroupID; }
    uint16  GetArrivedToTacticalPositionID() const { return m_arrivedToTacticalPositionID; }
    FVector GetTargetPlayerPos() const;

public:

    enum PlayerInteractionBehavior
    {
        PlayerInteractionBehavior_Collect,
        PlayerInteractionBehavior_Chase,
        PlayerInteractionBehavior_Flee
    };

    void GetHightestPriorityDetectionHit(const TArray<FHitResult>& hits, FHitResult& outDetectionHit);
    void UpdatePlayerInteractionBehavior(const FHitResult& detectionHit, float deltaTime);
    PlayerInteractionBehavior GetCurrentPlayerInteractionBehavior(const FHitResult& hit);
    bool HasLoSOnHit(const FHitResult& hit);
    void MoveToRandomCollectible();
    void MoveToPlayer();
    void MoveToBestFleeLocation();
    bool PlayerInteractionLoSUpdate();
    void OnPlayerInteractionNoLosDone();
    void OnMoveToTarget();

public:
    virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
    void RotateTowards(const FVector& targetLocation);
    void SetActorLocation(const FVector& targetLocation);
    void AIStateInterrupted(bool HasDied);
    bool IsTargetPlayerSeen() { return m_IsPlayerDetected; }
    
    UFUNCTION()
    void SetIsTargetPlayerSeen(bool IsPlayerSeen) { m_IsPlayerDetected = IsPlayerSeen; }

    UFUNCTION()
    void ShouldGoToFleeLocation(bool GoToFleeLocation);

    bool m_ReachedTarget;
    //bool m_ShouldGoToFleeLocation;

    FVector TargetPos;
    FVector FinalTargetPosForMoveTo;

private:
    virtual void GoToBestTarget(float deltaTime) override;
    virtual void UpdatePlayerInteraction(float deltaTime) override;
    virtual void ShowNavigationPath() override;


protected:
    FVector m_JumpTarget;
    FRotator m_ObstacleAvoidanceRotation;
    FTimerHandle m_PlayerInteractionNoLosTimer;
    PlayerInteractionBehavior m_PlayerInteractionBehavior;
    
    UPROPERTY(EditAnywhere, category = Behavior)
    UBehaviorTree* m_aiBehaviorTree;

private:

    bool m_IsPlayerDetected;

    UPROPERTY(transient)
    UBehaviorTreeComponent* m_behaviorTreeComponent;

    UPROPERTY(transient)
    UBlackboardComponent* m_blackboardComponent;

    uint16  m_targetPosBBKeyID;
    uint16  m_isTargetSeenBBKeyID;
    uint16  m_isSelectedForTacticalGroupID;
    uint16  m_nextPatrolDestinationBBKeyID;
    uint16  m_currentPatrolDestinationBBKeyID;
    uint16  m_arrivedToTacticalPositionID;
};
