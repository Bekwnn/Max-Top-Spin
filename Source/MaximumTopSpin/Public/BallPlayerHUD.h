// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "BallPlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class MAXIMUMTOPSPIN_API ABallPlayerHUD : public AHUD
{
	GENERATED_BODY()

	/** Variable for storing the font */
	UPROPERTY()
	UFont* HUDFont;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> HealthBoxes;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> DashBars;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> LivesCounter;
	
	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	/** Playing state HUD */
	void DrawPlayingHUD();

	/** Game over HUD */
	void DrawGameOverHUD();
public:
	ABallPlayerHUD(const FObjectInitializer& ObjectInitializer);
};
