// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "TopSpinPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MAXIMUMTOPSPIN_API ATopSpinPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ATopSpinPlayerController(const FObjectInitializer& ObjectInitializer);

protected:
	/** sets up input */
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;
};
