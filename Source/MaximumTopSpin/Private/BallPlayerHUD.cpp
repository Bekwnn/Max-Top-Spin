// Fill out your copyright notice in the Description page of Project Settings.

#include "MaximumTopSpin.h"
#include "BallPlayerHUD.h"
#include "MaximumTopSpinGameState.h"
#include "MaximumTopSpinBall.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "Engine.h"

ABallPlayerHUD::ABallPlayerHUD(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UFont>HUDFontOb(TEXT("/Engine/EngineFonts/RobotoDistanceField"));
	HUDFont = HUDFontOb.Object;

	static ConstructorHelpers::FObjectFinder<UMaterial>BoxMatOb(TEXT("/Game/Materials/M_PlayerStatusBox"));
	static ConstructorHelpers::FObjectFinder<UMaterial>DashMatOb(TEXT("/Game/Materials/M_HealthBar"));
	static ConstructorHelpers::FObjectFinder<UMaterial>LivesMatOb(TEXT("/Game/Materials/M_LivesCounter"));

	//create instances
	for (int count = 0; count < 4; count++)
	{
		UMaterialInstanceDynamic* playerbox = UMaterialInstanceDynamic::Create(BoxMatOb.Object, this);
		HealthBoxes.Add(playerbox);

		UMaterialInstanceDynamic* playerdashbar = UMaterialInstanceDynamic::Create(DashMatOb.Object, this);
		DashBars.Add(playerdashbar);

		UMaterialInstanceDynamic* playerlives = UMaterialInstanceDynamic::Create(LivesMatOb.Object, this);
		LivesCounter.Add(playerlives);
	}
}

void ABallPlayerHUD::DrawHUD()
{
	Super::DrawHUD();

	AMaximumTopSpinGameState* GameState = GetWorld()->GetGameState<AMaximumTopSpinGameState>();
	if (GameState == NULL) return;

	/** if in 'play' game mode state */
	if (GameState->GetCurrentState() == EMaximumTopSpinPlayState::EPlaying)
		DrawPlayingHUD();

	/** if in 'game over' state */
	else if (GameState->GetCurrentState() == EMaximumTopSpinPlayState::EGameOver)
		DrawGameOverHUD();
}

void ABallPlayerHUD::DrawPlayingHUD()
{
	//get the screen dimensions
	FVector2D ScreenDimensions = FVector2D(Canvas->SizeX, Canvas->SizeY);

	AMaximumTopSpinGameState* GameState = GetWorld()->GetGameState<AMaximumTopSpinGameState>();
	TArray<AMaximumTopSpinBall*> Players;
	if (GameState) Players = GameState->ActivePlayerPawns;

	int count = 0;
	for (AMaximumTopSpinBall* player : Players)
	{
		int NumPlayersOffsetX = ScreenDimensions.X / GameState->ActivePlayerPawns.Num();
		int FlatOffsetX = NumPlayersOffsetX / 2;
		float HPPercent = player->stats.CurrentHealth / player->stats.MaxHealth;
		float DashPercent = ((float)player->stats.CurrentDashCharges + player->GetDashCooldownElapsed() / player->stats.DashCooldown) / (float)player->stats.MaxDashCharges;

		// 100-51 = white, 50-21 = yellow, 20-0 = red
		FColor HealthStatus;
		if (HPPercent > .5f) HealthStatus = FColor::White;
		else if (HPPercent > 0.2f) HealthStatus = FColor::Yellow;
		else HealthStatus = FColor::Red;

		/** health boxes and percents */
		HealthBoxes[count]->SetVectorParameterValue("Player Color", player->PlayerColor);
		DrawMaterialSimple(HealthBoxes[count], FlatOffsetX + NumPlayersOffsetX * count - 82, ScreenDimensions.Y - 150, 165, 150);
		DrawText(FString::Printf(TEXT("%%%3.0f"), HPPercent * 100), HealthStatus, FlatOffsetX + NumPlayersOffsetX * count - 82, ScreenDimensions.Y - 125, HUDFont, 3);

		/** dash bars */
		DashBars[count]->SetVectorParameterValue("Full Color", FLinearColor(player->PlayerColor) * 10);
		DashBars[count]->SetVectorParameterValue("Empty Color", FLinearColor(player->PlayerColor) * 5);
		DashBars[count]->SetScalarParameterValue("Segments", player->stats.MaxDashCharges);
		DashBars[count]->SetScalarParameterValue("Bar Percentage", DashPercent);
		DrawMaterialSimple(DashBars[count], FlatOffsetX + NumPlayersOffsetX * count - 82, ScreenDimensions.Y - 150, 165, 25);

		/** lives counters */
		LivesCounter[count]->SetVectorParameterValue("Player Color", player->PlayerColor);
		if (player->stats.Lives <= 5)
		{
			for (int i = 0; i < player->stats.Lives; i++)
				DrawMaterialSimple(LivesCounter[count], FlatOffsetX + NumPlayersOffsetX * count + i * 25 - 82, ScreenDimensions.Y - 175, 25, 25);
		}
		else
		{
			DrawMaterialSimple(LivesCounter[count], FlatOffsetX + NumPlayersOffsetX * count - 82, ScreenDimensions.Y - 175, 25, 25);
			DrawText(FString::Printf(TEXT("x%2d"), player->stats.Lives), FColor::White, FlatOffsetX + NumPlayersOffsetX * count + 25 - 82, ScreenDimensions.Y - 175, HUDFont);
		}

		//step
		count++;
	}
}

void ABallPlayerHUD::DrawGameOverHUD()
{
	DrawText(FString::Printf(TEXT("GAME OVER")), FColor::Red, Canvas->SizeX / 2 - 50, Canvas->SizeY / 2 - 25, HUDFont);
}