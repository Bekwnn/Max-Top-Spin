// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "MaximumTopSpinBall.h"
#include "Camera/CameraActor.h"
#include "TopSpinCamera.generated.h"

/**
 * 
 */
UCLASS()
class MAXIMUMTOPSPIN_API ATopSpinCamera : public ACameraActor
{
	GENERATED_BODY()
	
public:
	ATopSpinCamera(const FObjectInitializer& ObjectInitializer);
	
	void Tick(float deltaseconds) override;

	float DepthOffset;

	TArray<AMaximumTopSpinBall*> PlayersInScene;
};
