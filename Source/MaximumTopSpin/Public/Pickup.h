// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

/**
 * 
 */
UCLASS()
class MAXIMUMTOPSPIN_API APickup : public AActor
{
	GENERATED_BODY()

public:
	APickup(const FObjectInitializer& ObjectInitializer);

protected:
	bool bIsActive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ball, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Ball, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PickupMesh;
};
