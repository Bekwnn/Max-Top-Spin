// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UnrealNetwork.h"
#include "MaximumTopSpinBall.h"
#include "GameFramework/GameState.h"
#include "MaximumTopSpinGameState.generated.h"

enum class EMaximumTopSpinPlayState : short
{
	EPlaying,
	EGameOver,
	EUnknown
};

/**
 * 
 */
UCLASS()
class MAXIMUMTOPSPIN_API AMaximumTopSpinGameState : public AGameState
{
	GENERATED_BODY()

	void BeginPlay() override;
	
public:
	AMaximumTopSpinGameState(const FObjectInitializer& ObjectInitializer);

	/** number of teams in current game (doesn't deprecate when no players are left in a team) */
	UPROPERTY(Transient, Replicated)
	int32 NumTeams;

	/** array of team colors */
	FColor* TeamColors;

	/** time left for warmup / match */
	UPROPERTY(Transient, Replicated)
	int32 RemainingTime;

	/** is timer paused? */
	UPROPERTY(Transient, Replicated)
	bool bTimerPaused;
	
	/** pawns of active players */
	UPROPERTY(Transient, Replicated)
	TArray<AMaximumTopSpinBall*> ActivePlayerPawns;

	void AddPlayer(AMaximumTopSpinBall* pawn);

	void PlayerDied(AMaximumTopSpinBall* DeadPlayer);

	EMaximumTopSpinPlayState GetCurrentState() const;

private:
	void SetCurrentState(EMaximumTopSpinPlayState NewState);

	//UPROPERTY(Transient, Replicated)
	EMaximumTopSpinPlayState CurrentState;

};

FORCEINLINE EMaximumTopSpinPlayState AMaximumTopSpinGameState::GetCurrentState() const
{
	return CurrentState;
}