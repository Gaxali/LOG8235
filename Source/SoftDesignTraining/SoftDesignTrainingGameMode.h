// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "SDTAIController.h"
#include "SoftDesignTrainingGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTacticalChanged, bool, TacticalChange);


UCLASS(minimalapi)
class ASoftDesignTrainingGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASoftDesignTrainingGameMode();

    virtual void StartPlay() override;

	void PlayerSeenByAI(ASDTAIController* InstigatorAIController);

	UPROPERTY(BlueprintAssignable)
	FOnTacticalChanged OnPayerSeenChange;

private:
	bool PlayerSeenBroadcasted = false;

};



