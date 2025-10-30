// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "NavLinkProxyForJump.generated.h"

/**
 * 
 */
UCLASS()
class SOFTDESIGNTRAINING_API ANavLinkProxyForJump : public ANavLinkProxy
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void SmartLinkReachedForJump(AActor* MovingActor, const FVector& DestinationPoint);

private:
	float MaxWalkSpeed = 0.0f;
	
};
