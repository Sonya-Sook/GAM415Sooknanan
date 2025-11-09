// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProject415PickUpComponent.h"

UMyProject415PickUpComponent::UMyProject415PickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UMyProject415PickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UMyProject415PickUpComponent::OnSphereBeginOverlap);
}

void UMyProject415PickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	AMyProject415Character* Character = Cast<AMyProject415Character>(OtherActor);
	if(Character != nullptr)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
