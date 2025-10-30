// Fill out your copyright notice in the Description page of Project Settings.


#include "NavLinkProxyForJump.h"
#include "../SoftDesignTrainingMainCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void ANavLinkProxyForJump::BeginPlay()
{
	Super::BeginPlay();
    	
	OnSmartLinkReached.AddDynamic(this, &ANavLinkProxyForJump::SmartLinkReachedForJump);
}

void ANavLinkProxyForJump::SmartLinkReachedForJump(AActor* MovingActor, const FVector& DestinationPoint)
{
	ASoftDesignTrainingMainCharacter* SoftCharacter = Cast<ASoftDesignTrainingMainCharacter>(MovingActor);

	if (SoftCharacter)
	{
		MaxWalkSpeed = SoftCharacter->GetCharacterMovement()->MaxWalkSpeed;

		SoftCharacter->GetCharacterMovement()->MaxWalkSpeed = 0.0f;

        FTimerHandle ResumeHandle;
        GetWorldTimerManager().SetTimer(
            ResumeHandle,
            [this, SoftCharacter, DestinationPoint]()
            {
                SoftCharacter->GetCharacterMovement()->MaxWalkSpeed = this->MaxWalkSpeed;

                FVector Start = SoftCharacter->GetActorLocation();
                FVector End = DestinationPoint;
                FVector LaunchVelocity;
                
                bool bFound = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
                    this,
                    LaunchVelocity,
                    Start,
                    End,
                    0, /*= 0*/
                    0.5 /*= 0.5f */
                );
                               
                //UE_LOG(LogTemp, Display, TEXT("Smart link jump complete — FAILD."));
    
            },
            0.3f,   // delay in seconds
            false   // looping = false
        );

	}
}