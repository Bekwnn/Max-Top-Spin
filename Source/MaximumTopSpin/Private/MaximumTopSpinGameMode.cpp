// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "MaximumTopSpin.h"
#include "MaximumTopSpinGameMode.h"
#include "MaximumTopSpinGameState.h"
#include "TopSpinPlayerController.h"
#include "BallPlayerHUD.h"
#include "Engine.h"

AMaximumTopSpinGameMode::AMaximumTopSpinGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AMaximumTopSpinGameState::StaticClass();
	DefaultPawnClass = AMaximumTopSpinBall::StaticClass();
	HUDClass = ABallPlayerHUD::StaticClass();
	PlayerControllerClass = ATopSpinPlayerController::StaticClass();
}

void AMaximumTopSpinGameMode::StartNewPlayer(APlayerController* NewPlayer)
{
	Super::StartNewPlayer(NewPlayer);

	AMaximumTopSpinGameState* GameState = GetWorld()->GetGameState<AMaximumTopSpinGameState>();
	AMaximumTopSpinBall* playerball = Cast<AMaximumTopSpinBall>(NewPlayer->GetPawnOrSpectator());
	if (playerball) GameState->AddPlayer(playerball);
}

void AMaximumTopSpinGameMode::RestartPlayer(AController* player)
{
	Super::RestartPlayer(player);
	
	APlayerController* PC = Cast<APlayerController>(player);
	if (PC) PC->SetViewTarget(MainCamera);
}

void AMaximumTopSpinGameMode::RespawnPlayer(AMaximumTopSpinBall* Player)
{
	/** loop through spawns and find one furthest from any active players*/
	APlayerStart* BestSpawn = NULL;
	float BestSpawnMaximumDistance = 0.f;
	for (APlayerStart* SpawnPoint : PlayerStarts)
	{
		if (BestSpawn == NULL)	//use first SpawnPoint to start
		{
			BestSpawn = SpawnPoint;
			BestSpawnMaximumDistance = FVector::Dist(BestSpawn->GetActorLocation(), Player->GetActorLocation());
		}
		else
		{
			/** loop through active players to find the one closest to SpawnPoint */
			AMaximumTopSpinGameState* GameState = GetWorld()->GetGameState<AMaximumTopSpinGameState>();
			TArray<AMaximumTopSpinBall*> ActivePlayers;
			if (GameState) ActivePlayers = GameState->ActivePlayerPawns;

			float minimumDistance = -1.f;
			float distance;
			for (AMaximumTopSpinBall* ActivePlayer : ActivePlayers)
			{
				distance = FVector::Dist(ActivePlayer->GetActorLocation(), SpawnPoint->GetActorLocation());
				if (distance < minimumDistance || minimumDistance < 0)
				{
					minimumDistance = distance;
				}
			}

			/** if we've got a new furthest spawn point */
			if (minimumDistance > BestSpawnMaximumDistance)
			{
				BestSpawn = SpawnPoint;
				BestSpawnMaximumDistance = minimumDistance;
			}
		}
	}
	if (BestSpawn)	//double check for null
		Player->SetActorLocationAndRotation(BestSpawn->GetActorLocation(), FRotator::ZeroRotator);
}