// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SDTStateMachine.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class SOFTDESIGNTRAINING_API USDTStateMachine : public UObject
{
	GENERATED_BODY()
	
public:

	void Run();
};
