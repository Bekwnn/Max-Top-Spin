// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "TopSpinCamera.h"
#include "MaximumTopSpinBall.h"
#include "GameFramework/GameMode.h"
#include "MaximumTopSpinGameMode.generated.h"

UCLASS(minimalapi)
class AMaximumTopSpinGameMode : public AGameMode
{
	GENERATED_BODY()

	void StartNewPlayer(APlayerController* NewPlayer) override;

public:
	void RestartPlayer(AController* player) override;

	AMaximumTopSpinGameMode(const FObjectInitializer& ObjectInitializer);

	ATopSpinCamera* MainCamera;

	void RespawnPlayer(AMaximumTopSpinBall* Player);

	int NumberOfPlayers;
};


