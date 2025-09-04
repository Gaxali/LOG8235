// Fill out your copyright notice in the Description page of Project Settings.

#include "APhysicActor.h"
#include "SoftDesignTraining.h"

#include "PhysicsHelpers.h"
#include "Engine/OverlapResult.h"

// Sets default values
AAPhysicActor::AAPhysicActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    UCapsuleComponent* capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PrimitiveComponent0"));

    capsule->SetCapsuleSize( 50.0f, 80.0f);
    capsule->SetHiddenInGame(false);
    capsule->SetGenerateOverlapEvents(true);

    RootComponent = capsule;
}

// Called when the game starts or when spawned
void AAPhysicActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAPhysicActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    FTransform trans = RootComponent->GetComponentTransform();

    PhysicsHelpers helper = PhysicsHelpers(GetWorld());


    FVector start = trans.GetLocation() + trans.GetUnitAxis(EAxis::X)*RootComponent->Bounds.GetSphere().W;
    FVector end = trans.GetLocation() + trans.GetUnitAxis(EAxis::X)*1000.0f;
    TArray<FHitResult> outHits;
    helper.CastRay(start, end, outHits, true);


    TArray<FOverlapResult> outOverlaps;
    helper.SphereOverlap( start, 100.0f, outOverlaps, true );


    outHits.Empty();
    helper.SphereCast(start, end, 50.0f, outHits, true);


    //Translate
    FVector deltaX = FVector( 10.0f,0.0f,0.0f) * DeltaTime;

    trans.AddToTranslation( deltaX );
    RootComponent->SetWorldTransform( trans );

}

