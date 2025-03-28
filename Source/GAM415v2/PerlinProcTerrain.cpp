// Fill out your copyright notice in the Description page of Project Settings.

#include "PerlinProcTerrain.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
APerlinProcTerrain::APerlinProcTerrain()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    // Create the procedural mesh component and attach it to the root component
    ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>("Procedural Mesh");
    ProcMesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void APerlinProcTerrain::BeginPlay()
{
    Super::BeginPlay();

    // Generate the mesh data
    CreateVertices();
    CreateTraingles();

    // Create the mesh section using generated data
    ProcMesh->CreateMeshSection(sectionID, Vertices, Triangles, Normals, UV0, UpVertexColors, TArray<FProcMeshTangent>(), true);

    // Apply the material to the procedural mesh
    ProcMesh->SetMaterial(0, Mat);
}

// Called every frame (currently disabled for performance optimization)
void APerlinProcTerrain::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Modifies the mesh by lowering vertices near an impact point
void APerlinProcTerrain::AlterMesh(FVector impactPoint)
{
    for (int i = 0; i < Vertices.Num(); i++)
    {
        // Calculate the relative position of the impact point
        FVector tempVector = impactPoint - this->GetActorLocation();

        // Check if the vertex is within the modification radius
        if (FVector(Vertices[i] - tempVector).Size() < radius)
        {
            // Lower the vertex by the specified depth
            Vertices[i] = Vertices[i] - Depth;

            // Update the mesh section with modified vertices
            ProcMesh->UpdateMeshSection(sectionID, Vertices, Normals, UV0, UpVertexColors, TArray<FProcMeshTangent>());
        }
    }
}

// Generates the vertices for the procedural terrain based on Perlin noise
void APerlinProcTerrain::CreateVertices()
{
    for (int X = 0; X <= XSize; X++)
    {
        for (int Y = 0; Y <= YSize; Y++)
        {
            // Calculate the Z (height) value using Perlin noise
            float Z = FMath::PerlinNoise2D(FVector2D(X * NoiseScale + 0.1, Y * NoiseScale + 0.1)) * ZMultiplier;

            // Debug message to visualize Z values
            GEngine->AddOnScreenDebugMessage(-1, 999.0f, FColor::Yellow, FString::Printf(TEXT("Z %f"), Z));

            // Add the vertex to the list
            Vertices.Add(FVector(X * Scale, Y * Scale, Z));

            // Store the corresponding UV coordinate
            UV0.Add(FVector2D(X * UVScale, Y * UVScale));
        }
    }
}

// Generates the triangle indices for the procedural mesh
void APerlinProcTerrain::CreateTraingles()
{
    int Vertex = 0;

    for (int X = 0; X < XSize; X++)
    {
        for (int Y = 0; Y < YSize; Y++)
        {
            // Define two triangles to form a quad
            Triangles.Add(Vertex);
            Triangles.Add(Vertex + 1);
            Triangles.Add(Vertex + YSize + 1);

            Triangles.Add(Vertex + 1);
            Triangles.Add(Vertex + YSize + 2);
            Triangles.Add(Vertex + YSize + 1);

            Vertex++;
        }

        // Skip to the next row
        Vertex++;
    }
}
