// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "MyProject415Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	RootArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RootArrow"));

	RootComponent = BoxComponent;
	PortalMesh->SetupAttachment(BoxComponent);
	SceneCapture->SetupAttachment(PortalMesh);
	RootArrow->SetupAttachment(RootComponent);

	// We want the portal to ignore all collisions as it's just a visual effect
	PortalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();

	// Set up the overlap event
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverlapBegin);
	PortalMesh->SetHiddenInSceneCapture(true);

	// Assign the portal material to the portal mesh
	if (PortalMaterial) {
		PortalMesh->SetMaterial(0, PortalMaterial);
	}
	
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update the portal view each frame
	UpdatePortals();
}

void APortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if the overlapping actor is the player character
	AMyProject415Character* Character = Cast<AMyProject415Character>(OtherActor);

	// Teleport the character to the other portal's location if not already teleporting
	if (Character && OtherPortal && !Character->isTeleporting) {
		// Set the teleporting flag to true to prevent immediate re-teleportation
		Character->isTeleporting = true;

		// Teleport the character to the other portal's location
		FVector Location = OtherPortal->RootArrow->GetComponentLocation();
		Character->SetActorLocation(Location);

		// Set a timer to reset the teleporting flag after 1 second
		FTimerHandle TeleportTimerHandle;
		FTimerDelegate TimerDel;
		TimerDel.BindUFunction(this, FName("SetBool"), Character);
		GetWorld()->GetTimerManager().SetTimer(TeleportTimerHandle, TimerDel, 1, false);
	}
}

void APortal::SetBool(AMyProject415Character* Character)
{
	// Reset the teleporting flag to allow future teleportations
	if (Character) {
		Character->isTeleporting = false;
	}
}

void APortal::UpdatePortals()
{
	// Calculate the offset between the two portals
	FVector PortalOffset = this->GetActorLocation() - OtherPortal->GetActorLocation();
	
	// Get the player's camera location
	FVector CameraLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetTransformComponent()->GetComponentLocation();

	// Get the player's camera rotation
	FRotator CameraRotation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetTransformComponent()->GetComponentRotation();

	// Set the scene capture component's location and rotation to simulate looking through the portal
	FVector CombinedLocation = CameraLocation + PortalOffset;
	SceneCapture->SetWorldLocationAndRotation(CombinedLocation, CameraRotation);
}

