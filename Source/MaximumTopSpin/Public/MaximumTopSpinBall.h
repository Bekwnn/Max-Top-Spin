// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Pawn.h"
#include "MaximumTopSpinBall.generated.h"

USTRUCT()
struct FPlayerStats {
	GENERATED_USTRUCT_BODY()

	/** Health variables */
	UPROPERTY()
	float MaxHealth;

	UPROPERTY()
	float CurrentHealth;

	/** Dash variables */
	UPROPERTY()
	float DashCooldown;

	UPROPERTY()
	uint8 MaxDashCharges;

	UPROPERTY()
	uint8 CurrentDashCharges;

	/** gets the moveright and aimup axes values in DashHandoff */
	UPROPERTY()
	FVector2D AxesTilt;

	/** Indicates whether the ball can dash */
	UPROPERTY()
	bool bCanDash;

	/** used to make the player invulnerable to regular damage */
	UPROPERTY()
	bool bIsInvulnerable;

	UPROPERTY()
	uint8 Lives;
};

UCLASS(config=Game)
class AMaximumTopSpinBall : public APawn
{
	GENERATED_BODY()

	/** StaticMesh used for the ball */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ball, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* Ball;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ball, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	/** Particle used for dash trail */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ball, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* DashTrail;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ball, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* DashOne;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ball, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* CollisionSparks;

	/** last frame and current frame's velocity */
	FVector CurrentVelocity;
	FVector PreviousVelocity;

	void OnConstruction(const FTransform & Transform) override;

public:
	AMaximumTopSpinBall(const FObjectInitializer& ObjectInitializer);

	/** replicated struct of common player stats */
	UPROPERTY(Replicated, RepRetry)
	FPlayerStats stats;

	/** Horizontal impulse to apply when dashing */
	UPROPERTY(EditAnywhere, Category=Ball)
	float DashImpulse;

	/** Burst torque applied when dashing */
	UPROPERTY(EditAnywhere, Category = Ball)
	float DashTorque;

	/** Torque to apply when trying to roll ball */
	UPROPERTY(EditAnywhere, Category=Ball)
	float RollTorque;

	UPROPERTY(EditAnywhere, Category = Ball)
	FColor PlayerColor;

	void Tick(float deltaseconds) override;

	void Respawn();

	/** status changers */
	void UpdateMaterialColor();

	void EnableTrail();

	void EnableTrail(float time);

	void DisableTrail();

	void DisableDash(float time);

	float GetDashCooldownElapsed();

	void MakeInvulnerable(float time);

protected:
	void DashHandoff();

	/** Called for side to side input */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerMoveRight(float Val);

	/** Handle dash action. */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDash(const FVector2D& AxesTilt);

	/** Reset the cooldown on Dash */
	void RefreshDash();

	/** Function called on death */
	void Destroyed();

	/** Ends the player's invulnerability */
	void EndInvulnerable();

	/** sets bCanDash to true, called by a timer set up by DisableDash(time)*/
	void EnableDash();

	// AActor interface
	virtual void ReceiveHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	// End of AActor interface

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	FORCEINLINE class UStaticMeshComponent* GetBall() const { return Ball; }

	/** public function to inflict damage on a ball pawn*/
	void InflictDamage(float amount);
};
