// Fill out your copyright notice in the Description page of Project Settings.

#include "MaximumTopSpin.h"
#include "MarbCannon.h"
#include "MaximumTopSpinBall.h"
#include "Engine.h"

AMarbCannon::AMarbCannon(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CannonMesh(TEXT("/Game/Meshes/BallMesh"));
	PickupMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Cannon0"));
	PickupMesh->SetStaticMesh(CannonMesh.Object);
	PickupMesh->AttachTo(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> FiringPart(TEXT("/Game/Particles/PS_MarbCannon"));
	FiringEffect = FiringPart.Object;

	CollisionSphere->bGenerateOverlapEvents = true;
	TScriptDelegate<FWeakObjectPtr> onOverlapDelegate;
	onOverlapDelegate.BindUFunction(this, "OnBeginComponentOverlap");
	CollisionSphere->OnComponentBeginOverlap.Add(onOverlapDelegate);

	fPowerLevel = 8.0f;
}

void AMarbCannon::OnBeginComponentOverlap(AActor *Other, UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	AMaximumTopSpinBall* ballactor = Cast<AMaximumTopSpinBall>(Other);
	if (ballactor == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("CannonFailed (Not a player)"));
		return;
	}

	UGameplayStatics::SpawnEmitterAtLocation(this, FiringEffect, GetActorLocation(), GetActorRotation());

	UStaticMeshComponent* Ball = ballactor->GetBall();
	Ball->SetAllPhysicsLinearVelocity(FVector(0.f, 0.f, 0.f));  //reset velocities
	Other->SetActorLocation(this->GetActorLocation());			//teleport ball to center of marb-cannon
	const FVector Impulse = GetActorUpVector() * 100000 * fPowerLevel;
	Ball->AddImpulse(Impulse);
	ballactor->DisableDash(0.2f);
	ballactor->DisableTrail();
	if (fDashTrailTime > 0.f)
		ballactor->EnableTrail(fDashTrailTime);
}