// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetActor.generated.h"

UCLASS()
class SOFTDESIGNTRAINING_API ATargetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATargetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    void SetVisibilityInformation(bool isVisible, bool canShoot);

    // visible but not shootable
    UPROPERTY(EditAnywhere)
    class UMaterial* VisibleNotShootableMaterial;

    // shootable but not visible
    UPROPERTY(EditAnywhere)
    class UMaterial* ShootableMaterial;

    // Visible, shootable
    UPROPERTY(EditAnywhere)
    class UMaterial* VisibleMaterial;

    // not visible, not shootable
    UPROPERTY(EditAnywhere)
    class UMaterial* NonVisibleMaterial;

};
