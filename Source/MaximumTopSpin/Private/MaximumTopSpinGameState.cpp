// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "MaximumTopSpin.h"
#include "MaximumTopSpinGameState.h"
#include "MaximumTopSpinGameMode.h"


AMaximumTopSpinGameState::AMaximumTopSpinGameState(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	NumTeams = 0;
	FColor temptcolors[] = { FColor::Red, FColor::Blue, FColor::Yellow, FColor::Green };
	TeamColors = temptcolors;
	RemainingTime = 0;
	bTimerPaused = false;
}

void AMaximumTopSpinGameState::AddPlayer(AMaximumTopSpinBall* pawn)
{
	if (ActivePlayerPawns.Contains(pawn))	//check if player is local
		return;

	ActivePlayerPawns.Add(pawn);
}

void AMaximumTopSpinGameState::BeginPlay()
{
	Super::BeginPlay();

	CurrentState = EMaximumTopSpinPlayState::EPlaying;
}

void AMaximumTopSpinGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMaximumTopSpinGameState, NumTeams);
	DOREPLIFETIME(AMaximumTopSpinGameState, RemainingTime);
	DOREPLIFETIME(AMaximumTopSpinGameState, bTimerPaused);
	DOREPLIFETIME(AMaximumTopSpinGameState, ActivePlayerPawns);
	//DOREPLIFETIME(AMaximumTopSpinGameState, CurrentState);
}

void AMaximumTopSpinGameState::PlayerDied(AMaximumTopSpinBall* DeadPlayer)
{
	DeadPlayer->stats.Lives--;

	/** check remaining lives and respawn or remove */
	if (DeadPlayer->stats.Lives == 0)
		ActivePlayerPawns.Remove(DeadPlayer);
	else
		DeadPlayer->Respawn();	//let the ball handle resetting its vars to respawn

	if (ActivePlayerPawns.Num() == 1)
		SetCurrentState(EMaximumTopSpinPlayState::EGameOver);
}

void AMaximumTopSpinGameState::SetCurrentState(EMaximumTopSpinPlayState NewState)
{
	CurrentState = NewState;
	switch (NewState)
	{
	case EMaximumTopSpinPlayState::EPlaying:
		break;
	case EMaximumTopSpinPlayState::EGameOver:
		break;
	case EMaximumTopSpinPlayState::EUnknown:
	default:
		break; //do nothing
	}
}
