// Fill out your copyright notice in the Description page of Project Settings.


#include "PerlinProcTerrain.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
APerlinProcTerrain::APerlinProcTerrain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	ProcMesh->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void APerlinProcTerrain::BeginPlay()
{
	Super::BeginPlay();
	
	// Setup procedural mesh
	CreateVertices();
	CreateTriangles();
	ProcMesh->CreateMeshSection(sectionId, Vertices, Triangles, Normals, UV0, UpVertexColors, TArray<FProcMeshTangent>(), true);
	ProcMesh->SetMaterial(0, Material);
}

// Called every frame
void APerlinProcTerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APerlinProcTerrain::AfterMesh(FVector ImpactPoint)
{
	// Modify vertices based on impact point
	for (int i = 0; i < Vertices.Num(); i++)
	{
		// Calculate distance from impact point
		FVector tempVector = ImpactPoint - this->GetActorLocation();
		
		// If within radius
		if (FVector(Vertices[i] - tempVector).Size() < Radius)
		{
			// Lower the vertex
			Vertices[i] = Vertices[i] - Depth;
			
			// Update the mesh section
			ProcMesh->UpdateMeshSection(sectionId, Vertices, Normals, UV0, UpVertexColors, TArray<FProcMeshTangent>());
		}
	}
}

void APerlinProcTerrain::CreateVertices()
{
	// Loop through X and Y sizes to create vertices with Perlin noise for Z values
	for (int X = 0; X <= XSize; X++)
	{
		for (int Y = 0; Y <= YSize; Y++)
		{
			// Calculate Z value using Perlin noise
			float Z = FMath::PerlinNoise2D(FVector2D(X * NoiseScale + 0.1, Y * NoiseScale + 0.1)) * ZMultiplier;
			GEngine->AddOnScreenDebugMessage(-1, 999.f, FColor::Yellow, FString::Printf(TEXT("Z: %f"), Z));
			
			// Add vertex and UV coordinate
			Vertices.Add(FVector(X * Scale, Y * Scale, Z));
			UV0.Add(FVector2D(X * UVScale, Y * UVScale));
		}
	}
}

void APerlinProcTerrain::CreateTriangles()
{
	int Vertex = 0;

	// Loop through X and Y sizes to create triangles
	for (int X = 0; X < XSize; X++)
	{
		for (int Y = 0; Y < YSize; Y++)
		{
			// First triangle
			Triangles.Add(Vertex);
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 1);

			// Second triangle
			Triangles.Add(Vertex + 1);
			Triangles.Add(Vertex + YSize + 2);
			Triangles.Add(Vertex + YSize + 1);
			Vertex++;
		}
		Vertex++;
	}
}

