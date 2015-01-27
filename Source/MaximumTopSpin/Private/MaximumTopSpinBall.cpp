// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "MaximumTopSpin.h"
#include "MaximumTopSpinBall.h"
#include "MaximumTopSpinGameState.h"
#include "Engine.h"

AMaximumTopSpinBall::AMaximumTopSpinBall(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BallMesh(TEXT("/Game/Meshes/BallMesh.BallMesh"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DashTrailPart(TEXT("/Game/Particles/PS_DashTrail"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> CollisionPart(TEXT("/Game/Particles/PS_PlayerShatter"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DashOnePart(TEXT("/Game/Particles/PS_DashOne"));

	// Create mesh component for the ball
	Ball = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("Ball0"));
	Ball->SetStaticMesh(BallMesh.Object);
	Ball->BodyInstance.SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	Ball->SetSimulatePhysics(true);
	Ball->SetLockedAxis(ELockedAxis::X);
	Ball->SetAngularDamping(0.1f);
	Ball->SetLinearDamping(0.1f);
	Ball->BodyInstance.MassScale = 3.5f;
	Ball->BodyInstance.MaxAngularVelocity = 800.0f;
	Ball->SetNotifyRigidBodyCollision(true);
	Ball->SetMaterial(0, UMaterialInstanceDynamic::Create(Ball->GetMaterial(0), this));
	RootComponent = Ball;

	//network replication
	bReplicates = true;
	bReplicateMovement = true;

	//add particle components
	DashTrail = UGameplayStatics::SpawnEmitterAttached(DashTrailPart.Object, RootComponent);
	DashOne = UGameplayStatics::SpawnEmitterAttached(DashOnePart.Object, RootComponent);

	//set collision effect
	CollisionSparks = CollisionPart.Object;

	// Set up forces
	RollTorque = 50000000.0f;
	DashImpulse = 800000.0f;
	DashTorque = 20000000000.0f;
	
	stats.Lives = 6;

	//set health
	stats.MaxHealth = 100.0f;
	stats.CurrentHealth = 30.0f;

	//set dashes
	stats.DashCooldown = 2.5f;
	stats.MaxDashCharges = 2;
	stats.CurrentDashCharges = 2;

	stats.bCanDash = true;
}


void AMaximumTopSpinBall::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	InputComponent->BindAxis("MoveRight", this, &AMaximumTopSpinBall::ServerMoveRight);
	InputComponent->BindAxis("AimUp");

	InputComponent->BindAction("Dash", IE_Pressed, this, &AMaximumTopSpinBall::DashHandoff);
}

void AMaximumTopSpinBall::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMaximumTopSpinBall, stats);
}

bool AMaximumTopSpinBall::ServerMoveRight_Validate(float Val)
{
	return true;
}

void AMaximumTopSpinBall::ServerMoveRight_Implementation(float Val)
{
	const FVector Torque = FVector(-1.f * Val * RollTorque, 0.f, 0.f);
	Ball->AddTorque(Torque);
}

void AMaximumTopSpinBall::DashHandoff()
{
	if (stats.CurrentDashCharges > 0)
		ServerDash(FVector2D(InputComponent->GetAxisValue("MoveRight"), InputComponent->GetAxisValue("AimUp")));
}

bool AMaximumTopSpinBall::ServerDash_Validate(const FVector2D& AxesTilt)
{
	return (stats.CurrentDashCharges > 0);
}

void AMaximumTopSpinBall::ServerDash_Implementation(const FVector2D& AxesTilt)
{
	FVector Impulse = FVector(
		0.f,
		AxesTilt.X,
		AxesTilt.Y
	);
	FVector Torque = FVector(-stats.AxesTilt.X, 0.f, 0.f);
	FVector CurrentVelocity = Ball->GetPhysicsLinearVelocity();
	float CurrentVelocityMagnitude = CurrentVelocity.Size();	//store current velocity magnitude since we're normalizing it for the dot product

	Impulse.Normalize();
	Torque.Normalize();
	CurrentVelocity.Normalize();

	//add any current momentum to dash direction, else reset momentum
	float CVdotD = FVector::DotProduct(Impulse, CurrentVelocity);
	if (CVdotD > 0.f)
		Ball->SetAllPhysicsLinearVelocity(CVdotD * CurrentVelocityMagnitude * Impulse);
	else
		Ball->SetAllPhysicsLinearVelocity(FVector(0.f, 0.f, 0.f)); //reset velocity to give more twitchy movement

	Impulse *= DashImpulse;
	Torque *= DashTorque;
	Ball->AddImpulse(Impulse);
	Ball->AddTorque(Torque);

	/**set timer if it wasn't going*/
	if (stats.CurrentDashCharges == stats.MaxDashCharges)
		GetWorldTimerManager().SetTimer(this, &AMaximumTopSpinBall::RefreshDash, stats.DashCooldown, false);
		
	/** reduce charges */
	stats.CurrentDashCharges--;

	/** dash particle */
	if (stats.CurrentDashCharges == 0)
		DashOne->bSuppressSpawning = true;

	EnableTrail(1.0f);
}

void AMaximumTopSpinBall::RefreshDash()
{
	GetWorldTimerManager().ClearTimer(this, &AMaximumTopSpinBall::RefreshDash);
	if (stats.CurrentDashCharges < stats.MaxDashCharges)
	{
		GetWorldTimerManager().SetTimer(this, &AMaximumTopSpinBall::RefreshDash, stats.DashCooldown, false);
		stats.CurrentDashCharges++;

		/** set particle indicator */
		DashOne->bSuppressSpawning = false;
	}
}

void AMaximumTopSpinBall::ReceiveHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::ReceiveHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (Other && Other->ActorHasTag("Lava"))
	{
		InflictDamage(5.f);
		return;
	}
	AMaximumTopSpinBall* otherball = Cast<AMaximumTopSpinBall>(Other);
	if (otherball)
	{
		float PVdotN = FVector::DotProduct(PreviousVelocity.SafeNormal(), -HitNormal);
		if (PVdotN * PreviousVelocity.Size() > 500.f)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, CollisionSparks, HitLocation, HitNormal.Rotation());
			otherball->InflictDamage(PVdotN * PreviousVelocity.Size() / 100.f);
		}
	}
}

void AMaximumTopSpinBall::Tick(float deltaseconds)
{
	PreviousVelocity = CurrentVelocity;
	CurrentVelocity = Ball->GetPhysicsLinearVelocity();
}

void AMaximumTopSpinBall::InflictDamage(float amount)
{
	if (!stats.bIsInvulnerable && stats.CurrentHealth >= 0.5f) {
		stats.CurrentHealth -= amount;
		MakeInvulnerable(0.2f);
	}

	//check for death, update visual appearance
	if (stats.CurrentHealth < 0.5f)
	{
		stats.CurrentHealth = 0.0f;
		Destroyed();	//should be called by gamemode when player is out of health
	}
}

void AMaximumTopSpinBall::Destroyed()
{
	AMaximumTopSpinGameState* GameState = GetWorld()->GetGameState<AMaximumTopSpinGameState>();
	if (GameState) GameState->PlayerDied(this);
}

void AMaximumTopSpinBall::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);

	AMaximumTopSpinGameState* GameState = GetWorld()->GetGameState<AMaximumTopSpinGameState>();
	if (GameState) GameState->AddPlayer(this);

	DisableTrail();	//turn off the trail particle to start
}

float AMaximumTopSpinBall::GetDashCooldownElapsed()
{
	if (GetWorldTimerManager().IsTimerActive(this, &AMaximumTopSpinBall::RefreshDash))
		return GetWorldTimerManager().GetTimerElapsed(this, &AMaximumTopSpinBall::RefreshDash);
	else
		return 0.f;
}

void AMaximumTopSpinBall::MakeInvulnerable(float time)
{
	stats.bIsInvulnerable = true;
	Cast<UMaterialInstanceDynamic>(Ball->GetMaterial(0))->SetScalarParameterValue("Invulnerable", 1.f);
	if (GetWorldTimerManager().IsTimerActive(this, &AMaximumTopSpinBall::EndInvulnerable))
	{
		if (GetWorldTimerManager().GetTimerRemaining(this, &AMaximumTopSpinBall::EndInvulnerable) > time)
			return;
	}
	GetWorldTimerManager().SetTimer(this, &AMaximumTopSpinBall::EndInvulnerable, time, false);
}

void AMaximumTopSpinBall::EndInvulnerable()
{
	stats.bIsInvulnerable = false;
	Cast<UMaterialInstanceDynamic>(Ball->GetMaterial(0))->SetScalarParameterValue("Invulnerable", 0.f);
}

void AMaximumTopSpinBall::DisableDash(float time)
{
	stats.bCanDash = false;
	if (GetWorldTimerManager().IsTimerActive(this, &AMaximumTopSpinBall::EnableDash))
	{
		if (GetWorldTimerManager().GetTimerRemaining(this, &AMaximumTopSpinBall::EnableDash) > time)
			return;
	}
	GetWorldTimerManager().SetTimer(this, &AMaximumTopSpinBall::EnableDash, time, false);
}

void AMaximumTopSpinBall::EnableDash()
{
	stats.bCanDash = true;
}

void AMaximumTopSpinBall::EnableTrail()
{
	DashTrail->bSuppressSpawning = false;
}

void AMaximumTopSpinBall::EnableTrail(float time)
{
	EnableTrail();
	if (GetWorldTimerManager().IsTimerActive(this, &AMaximumTopSpinBall::DisableTrail))
	{
		if (GetWorldTimerManager().GetTimerRemaining(this, &AMaximumTopSpinBall::DisableTrail) > time)
			return;
	}
	GetWorldTimerManager().SetTimer(this, &AMaximumTopSpinBall::DisableTrail, time, false);
}

void AMaximumTopSpinBall::DisableTrail()
{
	DashTrail->bSuppressSpawning = true;
}

void AMaximumTopSpinBall::UpdateMaterialColor()
{
	UMaterialInstanceDynamic* thismat = Cast<UMaterialInstanceDynamic>(Ball->GetMaterial(0));
	if (thismat)
	{
		thismat->SetVectorParameterValue("Player Color", PlayerColor);
		Ball->SetMaterial(0, thismat);
		DashOne->SetColorParameter("Player Color", FLinearColor(PlayerColor)*20);
	}
}

void AMaximumTopSpinBall::Respawn()
{
	Ball->SetAllPhysicsAngularVelocity(FVector::ZeroVector);
	Ball->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
	stats.CurrentHealth = stats.MaxHealth;
	while (stats.CurrentDashCharges < stats.MaxDashCharges)
		RefreshDash();
	//AMaximumTopSpinGameMode* MyGameMode = GetWorld()->GetAuthGameMode<AMaximumTopSpinGameMode>();
	//if (MyGameMode) MyGameMode->RespawnPlayer(this);
}