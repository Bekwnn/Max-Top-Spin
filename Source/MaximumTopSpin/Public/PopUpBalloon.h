// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "PopUpBalloon.generated.h"

/**
 * 
 */
UCLASS()
class MAXIMUMTOPSPIN_API APopUpBalloon : public APickup
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ball, meta = (AllowPrivateAccess = "true"))
	class UMaterial* BalloonMat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ball, meta = (AllowPrivateAccess = "true"))
	class UMaterial* BalloonMatRespawning;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Ball, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* FiringEffect;

public:
	APopUpBalloon(const FObjectInitializer& ObjectInitializer);

	void Tick(float deltaseconds) override;

protected:
	UPROPERTY(EditAnywhere, Category = PowerUp)
	float fPowerLevel;	//determines how strong the bounce up is

	UPROPERTY(EditAnywhere, Category = PowerUp)
	float fTimeToRespawn;

	bool bRespawning;

	UFUNCTION()
	void OnBeginComponentOverlap(AActor *Other, UPrimitiveComponent *OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	void DestroyBalloon();

	void Respawn();
	
};
