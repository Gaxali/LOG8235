// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTAIController.h"
#include "SoftDesignTraining.h"

#include "Algo/RandomShuffle.h"
#include "HAL/PlatformTime.h" // for FPlatformTime::Cycles

#include "SDTStateMachine.h"
#include "Engine/CollisionProfile.h"

#include "SoftDesignTrainingMainCharacter.h"
#include "SDTCollectible.h"
#include "EngineUtils.h"
ASDTAIController::ASDTAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    StateMachine = CreateDefaultSubobject<USDTStateMachine>("AIStateMachine");
}

void ASDTAIController::BeginPlay()
{
    Super::BeginPlay();

    auto AIPawn = GetPawn();

    if (AIPawn)
    {
        CharacterAI = Cast<ACharacter>(AIPawn);
        CharacterAI->GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
    }
}

bool ASDTAIController::TestRaycast(float AngleSideDegrees, float AngleDegreesDown, float Length, FHitResult& ResultOut, FColor Color)
{
    bool Success = false;

    FCollisionObjectQueryParams QueryParams;
    // Add walls and obstacles
    QueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

    // Add DeathObjects
    FName ChannelName(TEXT("DeathObject"));
    
    FCollisionResponseTemplate Response;
    if (UCollisionProfile::Get()->GetProfileTemplate(ChannelName, Response))
    {
        QueryParams.AddObjectTypesToQuery(Response.ObjectType);
    }
    //
 
    auto ForwardVector = GetPawn()->GetActorForwardVector();

    FVector Up(0, 0, 1);
    FVector VectorRotated = ForwardVector.RotateAngleAxis(AngleSideDegrees, Up);
    VectorRotated = VectorRotated.RotateAngleAxis(AngleDegreesDown, FVector::CrossProduct(Up, VectorRotated));

    auto PositionStart = GetPawn()->GetActorLocation();
    auto PositionEnd = GetPawn()->GetActorLocation() + Length * VectorRotated;

    Success = GetWorld()->LineTraceSingleByObjectType(ResultOut,
        PositionStart,
        PositionEnd,
        QueryParams);

    DrawDebugDirectionalArrow(GetWorld(), PositionStart, PositionEnd, 2, Color, false);

    return Success;
}

void ASDTAIController::AddMovement(FVector NewDirection)
{
    GetPawn()->AddMovementInput(NewDirection, 1);
    GetPawn()->SetActorRotation(NewDirection.ToOrientationQuat());
}

void ASDTAIController::AddMovementSides(const FHitResult& ResultIn, float Direction)
{
    int Factor = FMath::RandRange(0, 20) / 100;

    auto TangenDirection = Direction * FVector::CrossProduct(GetPawn()->GetActorUpVector(), ResultIn.ImpactNormal);
    auto NewDirection = (TangenDirection + ResultIn.ImpactNormal * Factor).GetSafeNormal();
    AddMovement(NewDirection);
}

bool ASDTAIController::HitObjectByChannelName(FHitResult& Result, FName& ChannelName)
{
    const UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(Result.GetComponent());
    if (!Component) return false;

    FCollisionResponseTemplate Response;
    if (UCollisionProfile::Get()->GetProfileTemplate(ChannelName, Response))
    {
        return Component->GetCollisionObjectType() == Response.ObjectType;
    }
        
    return false;
}

void ASDTAIController::AvoidingObstaces()
{
    FHitResult ResultLeftRay;
    FHitResult ResultRightRay;
    
    FName ChannelName(TEXT("DeathObject"));
    float Angle = 20.0f;

    bool CollisionDetectionLeftRay = TestRaycast(-Angle, 35, 200, ResultLeftRay);
    bool CollisionDetectionRightRay = TestRaycast(Angle, 35, 200, ResultRightRay);

    bool ResultLeftRayHitDeathFloor = HitObjectByChannelName(ResultLeftRay, ChannelName);
    bool ResultRightRayHitDeathFloor = HitObjectByChannelName(ResultRightRay, ChannelName);

    if (CollisionDetectionLeftRay && CollisionDetectionRightRay)
    {
        FVector TotalDirection = (ResultLeftRay.ImpactNormal + ResultRightRay.ImpactNormal).GetSafeNormal();

        if (ResultLeftRayHitDeathFloor || ResultRightRayHitDeathFloor)
        {
            TotalDirection = -GetPawn()->GetActorForwardVector();
        }

        int AngleDeg = FMath::RandRange(-10, 10);
        FVector Up(0, 0, 1);
        FVector VectorRotated = TotalDirection.RotateAngleAxis(AngleDeg, Up);

        AddMovement(VectorRotated);
    }
    else if (CollisionDetectionLeftRay)
    {
        if (ResultLeftRayHitDeathFloor)
        {
            AddMovement(-GetPawn()->GetActorForwardVector());
        }
        else
        {
            AddMovementSides(ResultLeftRay, -1);
        }
        
    }
    else if (CollisionDetectionRightRay)
    {
        if (ResultRightRayHitDeathFloor)
        {
            AddMovement(-GetPawn()->GetActorForwardVector());
        }
        else
        {
            AddMovementSides(ResultRightRay, 1);
        }
    }
    else
    {
        AddMovement(GetPawn()->GetActorForwardVector());
    }
}

void ASDTAIController::SpeedAdjustment()
{
    if (!CharacterAI)
    {
        return;
    }

    FHitResult Result;
    float MaxLenghtOfTheRayCast = 300.0f;
    float Distance = MaxLenghtOfTheRayCast;

    if (TestRaycast(0, 0, 300, Result, FColor::Blue))
    {
        Distance = Result.Distance;

    }

    CharacterAI->GetCharacterMovement()->MaxWalkSpeed = (Distance / MaxLenghtOfTheRayCast) * MaxWalkSpeed;
}

bool ASDTAIController::IsCharacterClose(ACharacter* TargetCharacter)
{
    FVector AIPosition = GetPawn()->GetActorLocation();
    FVector PlayerPosition = TargetCharacter->GetActorLocation();

    float SphereRadius = 500.0f;

    return FVector::Dist(AIPosition, PlayerPosition) <= SphereRadius;
}

bool ASDTAIController::IsCharacterInSight(ACharacter* TargetCharacter)
{
    FVector AIPosition = GetPawn()->GetActorLocation();
    FVector PlayerPosition = TargetCharacter->GetActorLocation();

    FHitResult HitResult;
    FCollisionObjectQueryParams QueryParams(FCollisionObjectQueryParams::AllStaticObjects);

    bool bHit = GetWorld()->LineTraceSingleByObjectType(
        HitResult,
        AIPosition,
        PlayerPosition,
        QueryParams
    );


    if (bHit)
        return false;
    return true;
}

void ASDTAIController::FindPickup()
{
    FVector AIPosition = GetPawn()->GetActorLocation();
    float MinDistance = 750.0f;
	ClosestPickupPosition = FVector::ZeroVector;

    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        ASDTCollectible* Pickup = Cast<ASDTCollectible>(*It);
        if (Pickup && !Pickup->IsOnCooldown())
        {
            FVector PickupPosition = Pickup->GetActorLocation();
            float Distance = FVector::Dist(AIPosition, PickupPosition);

            if (Distance < MinDistance)
            {
                FHitResult HitResult;
                FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::AllObjects);
                FCollisionQueryParams QueryParams;
                QueryParams.AddIgnoredActor(GetPawn());

                bool bHit = GetWorld()->LineTraceSingleByObjectType(
                    HitResult,
                    AIPosition,
                    PickupPosition,
                    ObjectQueryParams,
                    QueryParams
                );
				UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), bHit ? *HitResult.GetActor()->GetName() : TEXT("None"));
                if (bHit && HitResult.GetActor() == Pickup)
                {
                    ClosestPickupPosition = PickupPosition;
                }
            }
        }
    }
}

void ASDTAIController::UpdateReferencePosition(ACharacter* TargetCharacter)
{
    FVector PlayerPosition = TargetCharacter->GetActorLocation();
    if (IsCharacterInSight(TargetCharacter) && FVector::Dist(ReferencePlayerPosition, PlayerPosition) > 10.0f)
        ReferencePlayerPosition = PlayerPosition;
}

void ASDTAIController::Chase()
{
    ACharacter* PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    UpdateReferencePosition(PlayerCharacter);
    FVector AIPosition = GetPawn()->GetActorLocation();
    FVector DirectionVector = (ReferencePlayerPosition - AIPosition).GetSafeNormal();
	AddMovement(DirectionVector);
}

void ASDTAIController::Flee()
{
    ACharacter* PlayerCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    FVector AIPosition = GetPawn()->GetActorLocation();
    FVector PlayerPosition = PlayerCharacter->GetActorLocation();
    FVector DirectionVector = (PlayerPosition - AIPosition).GetSafeNormal();
    AddMovement(-DirectionVector);
}

void ASDTAIController::Collect()
{
    FVector AIPosition = GetPawn()->GetActorLocation();
    FVector DirectionVector = (ClosestPickupPosition - AIPosition).GetSafeNormal();
    AddMovement(DirectionVector);
}

void ASDTAIController::Transition()
{
    ASoftDesignTrainingMainCharacter* PlayerCharacter = Cast<ASoftDesignTrainingMainCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    bool bIsClose = IsCharacterClose(PlayerCharacter);
    bool bInSight = IsCharacterInSight(PlayerCharacter);
	bool bplayerHasPickup = PlayerCharacter->IsPoweredUp();

    switch (CurrentState)
    {
        case AIState::Patrol:
            if (bIsClose && bInSight && !bplayerHasPickup)
            {
                CurrentState = AIState::Chase;
            }
            if (bIsClose && bInSight && bplayerHasPickup)
            {
                CurrentState = AIState::Flee;
		    }
            FindPickup();
            if (ClosestPickupPosition != FVector::ZeroVector)
            {
                CurrentState = AIState::Collect;
			}
            break;
        case AIState::Chase:
            if (!bIsClose)
            {
                CurrentState = AIState::Patrol;
            }
            if (bIsClose && bInSight && bplayerHasPickup)
            {
                CurrentState = AIState::Flee;
		    }
            break;
        case AIState::Flee:
            if (!bIsClose)
            {
                CurrentState = AIState::Patrol;
            }
            if (bIsClose && bInSight && !bplayerHasPickup)
            {
                CurrentState = AIState::Chase;
		    }
            break;
		case AIState::Collect:
            if (bIsClose && bInSight && !bplayerHasPickup)
            {
                CurrentState = AIState::Chase;
            }
            if (bIsClose && bInSight && bplayerHasPickup)
            {
                CurrentState = AIState::Flee;
            }
			FindPickup();
            if (ClosestPickupPosition == FVector::ZeroVector)
            {
				CurrentState = AIState::Patrol;
            }
            break;
        default:
            break;
     }
}
void ASDTAIController::NavigationPatrol()
{
    AvoidingObstaces();

    SpeedAdjustment();
}

void ASDTAIController::Move()
{
    switch (CurrentState)
    {
    case AIState::Patrol:
        NavigationPatrol();
        break;
    case AIState::Chase:
        Chase();
        break;
    case AIState::Flee:
        Flee();
        break;
	case AIState::Collect:
		Collect();
		break;
    default:
        break;
    }
}

void ASDTAIController::Tick(float deltaTime)
{  
    Transition();
    Move();
    FVector AIPosition = GetPawn()->GetActorLocation();

    float SphereRadius = 500.0f;
    DrawDebugSphere(GetWorld(), AIPosition, SphereRadius, 16, FColor::Green, false, -1, 0, 2);
	Super::Tick(deltaTime);

    if (StateMachine)
    {
        StateMachine->Run();
    }
}




