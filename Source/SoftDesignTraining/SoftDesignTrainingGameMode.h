// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "SDTAIController.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "SoftDesignTrainingGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTacticalChanged, bool, TacticalChange);

class UEnvQuery;

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

protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* TacticalAttackPositionsQuery;

	UFUNCTION()
	void OnQueryTacticalPositionsAttack(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

private:
	bool PlayerSeenBroadcasted = false;

};



