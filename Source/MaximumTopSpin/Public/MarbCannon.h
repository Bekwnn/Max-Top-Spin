// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "MarbCannon.generated.h"

/**
 * 
 */
UCLASS()
class MAXIMUMTOPSPIN_API AMarbCannon : public APickup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ball, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* FiringEffect;
	
public:
	AMarbCannon(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(EditAnywhere, Category = PowerUp)
	float fPowerLevel;	//determines how strong the bounce up is

	UPROPERTY(EditAnywhere, Category = PowerUp)
	float fDashTrailTime;	//how long the player's dash trail should last when launched

	UFUNCTION()
	void OnBeginComponentOverlap(AActor *Other, UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
};
