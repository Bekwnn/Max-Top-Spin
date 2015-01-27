// Fill out your copyright notice in the Description page of Project Settings.

#include "MaximumTopSpin.h"
#include "TopSpinPlayerController.h"
#include "TopSpinCamera.h"
#include "Engine.h"
#include "MaximumTopSpinGameState.h"


ATopSpinPlayerController::ATopSpinPlayerController(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bReplicates = true;
	bAutoManageActiveCameraTarget = false;
}

void ATopSpinPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// voice chat
	InputComponent->BindAction("PushToTalk", IE_Pressed, this, &APlayerController::StartTalking);
	InputComponent->BindAction("PushToTalk", IE_Released, this, &APlayerController::StopTalking);
}

void ATopSpinPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetViewTarget(GetWorld()->SpawnActor<ATopSpinCamera>());
}