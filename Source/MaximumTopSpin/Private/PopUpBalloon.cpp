// Fill out your copyright notice in the Description page of Project Settings.

#include "MaximumTopSpin.h"
#include "PopUpBalloon.h"
#include "MaximumTopSpinBall.h"
#include "Engine.h"

APopUpBalloon::APopUpBalloon(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BalloonMesh(TEXT("/Game/Meshes/BallMesh.BallMesh"));

	PickupMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Balloon0"));
	PickupMesh->SetStaticMesh(BalloonMesh.Object);
	PickupMesh->AttachTo(RootComponent);

	//set up overlap collision and delegate
	TScriptDelegate<FWeakObjectPtr> onOverlapDelegate;
	onOverlapDelegate.BindUFunction(this, "OnBeginComponentOverlap");
	CollisionSphere->OnComponentBeginOverlap.Add(onOverlapDelegate);

	PrimaryActorTick.bCanEverTick = true;
	fPowerLevel = 5.0f;
	fTimeToRespawn = 5.0f;
}

void APopUpBalloon::OnBeginComponentOverlap(AActor *Other, UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (bRespawning)
		return;

	//check for ball actor
	AMaximumTopSpinBall* ballactor = Cast<AMaximumTopSpinBall>(Other);
	if (ballactor == NULL) {
		UE_LOG(LogTemp, Warning, TEXT("PopFailed (Not a player)"));
		return;
	}
	
	//reset Z axis velocity if it's below 0
	UStaticMeshComponent* Ball = ballactor->GetBall();
	FVector current_velocity = Ball->GetPhysicsLinearVelocity();
	current_velocity = (current_velocity.Z < 0.f)? FVector(0.f, current_velocity.Y, 0.f) : FVector(0.f, current_velocity.Y, current_velocity.Z);
	Ball->SetAllPhysicsLinearVelocity(FVector(0.f, current_velocity.Y, current_velocity.Z));

	//apply impulse
	const FVector Impulse = FVector(0.f, 0.f, fPowerLevel * 100000);
	Ball->AddImpulse(Impulse);

	DestroyBalloon();
}

void APopUpBalloon::DestroyBalloon()
{
	//add scaling respawn
	UGameplayStatics::SpawnEmitterAtLocation(this, FiringEffect, GetActorLocation(), GetActorRotation());
	PickupMesh->SetMaterial(0, BalloonMatRespawning);
	PickupMesh->SetRelativeScale3D(FVector::ZeroVector);
	bRespawning = true;
	GetWorldTimerManager().SetTimer(this, &APopUpBalloon::Respawn, fTimeToRespawn, false);
}

void APopUpBalloon::Tick(float deltaseconds)
{
	if (bRespawning)
	{
		PickupMesh->SetRelativeScale3D(FVector(GetWorldTimerManager().GetTimerElapsed(this, &APopUpBalloon::Respawn) / fTimeToRespawn));
	}
}

void APopUpBalloon::Respawn()
{
	PickupMesh->SetMaterial(0, BalloonMat);
	PickupMesh->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
	bRespawning = false;
}

