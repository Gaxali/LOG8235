// Fill out your copyright notice in the Description page of Project Settings.

#include "AIBase.h"
#include "SoftDesignTraining.h"
#include "DrawDebugHelpers.h"
#include "ReactionManager.h"
#include "PatrolPathManager.h"
#include "PatrolPathComponent.h"
#include "ObjectPartition.h"
#include "SDTUtils.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

// Sets default values
AAIBase::AAIBase()
{
     // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

    PrimaryActorTick.bCanEverTick = true;
    m_CurrentWayPoint = 0;
    m_PatrolPath = nullptr;
    
    m_IsBlind = false;
    m_IsPlayerDetected = false;
}

// Called when the game starts or when spawned
void AAIBase::BeginPlay()
{
    Super::BeginPlay();

    ReactionManager* reactionManager = ReactionManager::GetInstance();
    if (reactionManager)
    {
        reactionManager->RegisterNPC(this);
    }

    PatrolPathManager* ppm = PatrolPathManager::GetInstance();
    if (ppm)
    {
        m_PatrolPath = ppm->GetRandomPath();
        m_CurrentWayPoint = rand() % m_PatrolPath->GetNWayPoints();
        //m_PathDirection = (rand() & 0x1) ? 1 : -1;
        m_PathDirection = 1;
    }

    GetCharacterMovement()->MaxWalkSpeed = (float)(rand() % 200 + 100);

    FVector actorLocation = GetActorLocation();
    m_NextDestination = actorLocation;
    m_CurrentDestination = actorLocation;
}


void AAIBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    ReactionManager* reactionManager = ReactionManager::GetInstance();
    if (reactionManager)
    {
        reactionManager->UnregisterNPC(this);
    }

    m_PatrolPath = nullptr;
}

// Called every frame
void AAIBase::Tick( float DeltaTime )
{
    Super::Tick( DeltaTime );

    DetectPlayer();

    FVector actorLocation = GetActorLocation();
    DrawDebugLine(GetWorld(), actorLocation, FVector::ZeroVector, FColor::Green, false);

    if (m_IsBlind)
    {
        if (m_BlindTimeLeft > 0)
        {
            m_BlindTimeLeft -= DeltaTime;
            SetNewMoveDestination(actorLocation);
            return;
        }
        else
        {
            m_BlindTimeLeft = 0.0f;
            m_IsBlind = false;

            if (m_PatrolPath)
            {
                m_PatrolPath->GetNextWayPoint(m_CurrentWayPoint, m_NextDestination, m_PathDirection);
                m_CurrentDestination = m_NextDestination;
                SetNewMoveDestination(m_NextDestination);
            }
        }
    }
    else if (m_IsPlayerDetected)
    {
        AActor* targetPlayer = NULL;

        ReactionManager* reactionManager = ReactionManager::GetInstance();
        if (reactionManager)
        {
            targetPlayer = reactionManager->GetPlayer();
        }
        
        if ( targetPlayer )
        {
            FVector npcPosition = GetActorLocation();
            FVector targetPosition = targetPlayer->GetActorLocation();
            FVector distToTarget = npcPosition - targetPosition;
            if (distToTarget.Size() < 250.0f)
            {
                SetNewMoveDestination(actorLocation);
            }
            else
            {
                SetNewMoveDestination(targetPosition);
            }

            m_CurrentDestination = FVector::ZeroVector;
            return;
        }
    }
    else if (m_CurrentDestination == FVector::ZeroVector)
    {
        m_CurrentDestination = m_NextDestination;
        SetNewMoveDestination(m_NextDestination);
    }
    
    // check if we are close enough to our next destination
    if ((actorLocation - m_NextDestination).Size2D() < 50.f)
    {
        // on est rendu!! ouais!
        if (m_PatrolPath)
        {
            m_PatrolPath->GetNextWayPoint(m_CurrentWayPoint, m_NextDestination, m_PathDirection);
            m_CurrentDestination = m_NextDestination;
            SetNewMoveDestination(m_NextDestination);
        }
    }

    ProcessAvoidance();
}

// Called to bind functionality to input
void AAIBase::SetupPlayerInputComponent(class UInputComponent* inputComponent)
{
    Super::SetupPlayerInputComponent(inputComponent);
}

void AAIBase::SetNewMoveDestination(const FVector DestLocation)
{
    UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), DestLocation);
}

void AAIBase::ProcessReaction(ReactionEvent* reactionEvent)
{
    if (!reactionEvent)
        return;

    FVector npcPosition = GetActorLocation();
    FVector npcHead = npcPosition + FVector::UpVector * 200.0f;
    UWorld* npcWorld = GetWorld();

    DrawDebugSphere(npcWorld, npcHead, reactionEvent->m_ReactionLOS == ReactionLOS_Visual ? 50.0f : 60.0f, 32, reactionEvent->m_ReactionLOS == ReactionLOS_Visual ? FColor::Green : FColor::Red, false, reactionEvent->m_ReactionLOS == ReactionLOS_Visual ? 3.0f : 0.5f);

    if (reactionEvent->m_ReactionLOS == ReactionLOS_Visual)
    {
        m_IsBlind = true;
        m_BlindTimeLeft = 3.0f;
    }
}

void AAIBase::ProcessAvoidance()
{
    FVector npcPosition = GetActorLocation();
    FVector npcHead = npcPosition + FVector::UpVector * 200.0f;
    UWorld* npcWorld = GetWorld();

    ObjectPartition* op = ObjectPartition::GetInstance();
    if (op)
    {
        TArray<AActor*> actors;
        op->GatherObjectsAroundPosition(this, npcPosition, 250.f, actors);
        if (actors.Num() > 0)
        {
            DrawDebugSphere(npcWorld, npcHead, 20.0f, 32, FColor::Magenta);
        }
    }
}

void AAIBase::DetectPlayer()
{
    m_IsPlayerDetected = false;
    AActor* targetPlayer = NULL;
    
    ReactionManager* reactionManager = ReactionManager::GetInstance();
    if (reactionManager)
    {
        targetPlayer = reactionManager->GetPlayer();
    }

    if (targetPlayer)
    {
        bool canSeePlayer = false;
        FVector npcPosition = GetActorLocation();
        FVector targetPosition = targetPlayer->GetActorLocation();
        FVector distToTarget = npcPosition - targetPosition;
        if (distToTarget.Size() < 1500.0f)
        {
            FVector npcHead = npcPosition + FVector::UpVector * 200.0f;
            FVector playerHead = targetPosition + FVector::UpVector * 200.0f;
            FVector playerTorso = targetPosition + FVector::UpVector * 100.0f;
            FVector playerFeet = targetPosition + FVector::UpVector * 25.0f;

            UWorld* npcWorld = GetWorld();
            int bodyPartSeen = 0;

            if (!SDTUtils::Raycast(npcWorld, npcHead, playerHead))
            {
                DrawDebugLine(npcWorld, npcHead, playerHead, FColor::Blue, false, 0.066f);
                ++bodyPartSeen;
            }
            if (!SDTUtils::Raycast(npcWorld, npcHead, playerTorso))
            {
                DrawDebugLine(npcWorld, npcHead, playerTorso, FColor::Blue, false, 0.066f);
                ++bodyPartSeen;
            }
            if (!SDTUtils::Raycast(npcWorld, npcHead, playerFeet))
            {
                DrawDebugLine(npcWorld, npcHead, playerFeet, FColor::Blue, false, 0.066f);
                ++bodyPartSeen;
            }

            if (bodyPartSeen > 1)
            {
                m_IsPlayerDetected = true;
            }
        }
    }
}