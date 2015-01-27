// Fill out your copyright notice in the Description page of Project Settings.

#include "MaximumTopSpin.h"
#include "TopSpinCamera.h"
#include "MaximumTopSpinGameState.h"

ATopSpinCamera::ATopSpinCamera(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	DepthOffset = 1200.f;
}

void ATopSpinCamera::Tick(float deltaseconds)
{
	AMaximumTopSpinGameState* GameState = GetWorld()->GetGameState<AMaximumTopSpinGameState>();

	if (GameState == NULL)
		return;

	PlayersInScene = GameState->ActivePlayerPawns;

	FVector BottomLeft;		//needs initializing to a min value
	FVector TopRight;		//needs initializing to a max value
	bool first = true;
	for (AMaximumTopSpinBall* ball : PlayersInScene)
	{
		//initialize top right and bottom left to first value
		if (first)
		{
			BottomLeft = ball->GetActorLocation();
			TopRight = ball->GetActorLocation();
			first = false;
		}

		//check for new mins/maxs
		FVector ballLocation = ball->GetActorLocation();
		if (ballLocation.Y < BottomLeft.Y)
			BottomLeft.Y = ballLocation.Y;
		else if (ballLocation.Y > TopRight.Y)
			TopRight.Y = ballLocation.Y;
		if (ballLocation.Z < BottomLeft.Z)
			BottomLeft.Z = ballLocation.Z;
		else if (ballLocation.Z > TopRight.Z)
			TopRight.Z = ballLocation.Z;
	}

	//set new location to be middle of rectangle (assumes 90 degree horizontal FOV and 16:9 aspect ratio)
	FVector NewLocation = (BottomLeft + TopRight) / 2;
	if (FMath::Abs(BottomLeft.Y - TopRight.Y) > FMath::Abs(BottomLeft.Z - TopRight.Z)*2.f)
		NewLocation.X = -DepthOffset - FMath::Abs(BottomLeft.Y - TopRight.Y) / 2.f;
	else
		NewLocation.X = -DepthOffset - FMath::Abs(BottomLeft.Z - TopRight.Z);
	
	//set lookat
	FVector NewLookat = FVector(-550.f, NewLocation.Y, NewLocation.Z);
	float upwardOffset = NewLocation.Size()/10;	//upward offset
	NewLocation.Z += upwardOffset;
	SetActorLocation(NewLocation);
	SetActorRotation((NewLookat - NewLocation).Rotation());
}

