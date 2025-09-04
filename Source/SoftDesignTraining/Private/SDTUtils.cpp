// Fill out your copyright notice in the Description page of Project Settings.

#include "SDTUtils.h"
#include "SoftDesignTraining.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "AIBase.h"

/*static*/ bool SDTUtils::Raycast(UWorld* uWorld, FVector sourcePoint, FVector targetPoint)
{
    FHitResult hitData;
    FCollisionQueryParams TraceParams(FName(TEXT("VictoreCore Trace")), true);
    
    // Fake cost for the exercise
    //Sleep(1);
    // End fake cost

    return uWorld->LineTraceSingleByChannel(hitData, sourcePoint, targetPoint, ECC_Pawn, TraceParams);
}