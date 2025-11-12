// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProject415Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


AMyProject415Projectile::AMyProject415Projectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AMyProject415Projectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Create the ball mesh component
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));

	// Set as root component
	RootComponent = CollisionComp;

	// Attach ball mesh to root component
	BallMesh->SetupAttachment(RootComponent);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 1.0f;
}

void AMyProject415Projectile::BeginPlay()
{
	Super::BeginPlay();
	
	// Generate a random color for the projectile and decal
	RandomColor = FLinearColor(
		UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f),
		UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f),
		UKismetMathLibrary::RandomFloatInRange(0.0f, 1.0f),
		1.0f
	);

	// Create dynamic material instance and set the color parameter for the ball mesh
	UMaterialInstanceDynamic* BallMeshDMI = UMaterialInstanceDynamic::Create(BallMaterial, this);
	BallMeshDMI->SetVectorParameterValue(TEXT("Color"), RandomColor);

	// Apply the dynamic material to the ball mesh
	BallMesh->SetMaterial(0, BallMeshDMI);
}

void AMyProject415Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}

	// If we hit something, and we have a decal material
	if (OtherActor != nullptr && DecalMaterial != nullptr)
	{
		if (ImpactEffect != nullptr) {
			UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
				ImpactEffect,
				HitComp,
				NAME_None,
				FVector(0.f),
				FRotator(0.f),
				EAttachLocation::KeepRelativeOffset,
				true
			);
			NiagaraComp->SetVariableLinearColor(TEXT("Color"), RandomColor);
			BallMesh->DestroyComponent();
			CollisionComp->BodyInstance.SetCollisionProfileName("NoCollision");
		}

		// Generate random values for color, frame, and size
		float ranNumFrame = UKismetMathLibrary::RandomFloatInRange(0.0f, 3.0f);
		float ranNumSize = UKismetMathLibrary::RandomFloatInRange(20.0f, 40.0f);

		// Spawn decal at hit location
		auto Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DecalMaterial, FVector(ranNumSize), Hit.Location, Hit.Normal.Rotation(), 0.f);

		// Create dynamic material instance and set parameters
		auto MaterialInstance = Decal->CreateDynamicMaterialInstance();

		// Set decal parameters
		MaterialInstance->SetVectorParameterValue(TEXT("Color"), RandomColor);
		MaterialInstance->SetScalarParameterValue(TEXT("Frame"), ranNumFrame);
	}
}