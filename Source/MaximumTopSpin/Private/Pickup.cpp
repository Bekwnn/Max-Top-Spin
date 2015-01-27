// Fill out your copyright notice in the Description page of Project Settings.

#include "MaximumTopSpin.h"
#include "Pickup.h"

APickup::APickup(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bIsActive = true;

	CollisionSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("CollisionSphere"));
	CollisionSphere->bGenerateOverlapEvents = true;
	RootComponent = CollisionSphere;
}


