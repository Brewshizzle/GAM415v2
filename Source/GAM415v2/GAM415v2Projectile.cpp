// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAM415v2Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "PerlinProcTerrain.h"

// Constructor for the projectile class
AGAM415v2Projectile::AGAM415v2Projectile()
{
    // Use a sphere as a simple collision representation
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(5.0f);
    CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

    // Set up a notification for when this component hits something blocking
    CollisionComp->OnComponentHit.AddDynamic(this, &AGAM415v2Projectile::OnHit);

    // Prevent players from walking on the projectile
    CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
    CollisionComp->CanCharacterStepUpOn = ECB_No;

    // Create a static mesh component for the projectile
    ballMesh = CreateDefaultSubobject<UStaticMeshComponent>("Ball Mesh");

    // Set the collision component as the root component
    RootComponent = CollisionComp;

    // Attach the ball mesh to the collision component
    ballMesh->SetupAttachment(CollisionComp);

    // Use a ProjectileMovementComponent to control movement behavior
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 3000.f;
    ProjectileMovement->MaxSpeed = 3000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = true;

    // Set the projectile to self-destruct after 3 seconds
    InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AGAM415v2Projectile::BeginPlay()
{
    Super::BeginPlay();

    // Generate a random color for the projectile
    randColor = FLinearColor(
        UKismetMathLibrary::RandomFloatInRange(0.f, 1.f),
        UKismetMathLibrary::RandomFloatInRange(0.f, 1.f),
        UKismetMathLibrary::RandomFloatInRange(0.f, 1.f),
        1.f
    );

    // Create a dynamic material instance and apply it to the projectile mesh
    dmiMat = UMaterialInstanceDynamic::Create(projMat, this);
    ballMesh->SetMaterial(0, dmiMat);

    // Set the material color parameter
    dmiMat->SetVectorParameterValue("ProjColor", randColor);
}

// Function called when the projectile collides with another object
void AGAM415v2Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // If the hit object has physics enabled, apply an impulse and destroy the projectile
    if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
    {
        OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
        Destroy();
    }

    // If the projectile hits any valid actor
    if (OtherActor != nullptr)
    {
        // Spawn a Niagara particle effect at the hit location if colorP is assigned
        if (colorP)
        {
            UNiagaraComponent* particleComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
                colorP,
                HitComp,
                NAME_None,
                FVector(-20.f, 0.f, 0.f),
                FRotator(0.f),
                EAttachLocation::KeepRelativeOffset,
                true
            );

            // Set the color of the particle effect
            particleComp->SetNiagaraVariableLinearColor(FString("RandomColor"), randColor);

            // Destroy the projectile's mesh component and disable collision
            ballMesh->DestroyComponent();
            CollisionComp->BodyInstance.SetCollisionProfileName("NoCollision");
        }

        // Generate a random animation frame for the decal effect
        float frameNum = UKismetMathLibrary::RandomFloatInRange(0.f, 3.f);

        // Spawn a decal at the impact location
        auto Decal = UGameplayStatics::SpawnDecalAtLocation(
            GetWorld(),
            baseMat,
            FVector(UKismetMathLibrary::RandomFloatInRange(20.f, 40.f)),
            Hit.Location,
            Hit.Normal.Rotation(),
            0.f
        );

        // Modify the decal material properties dynamically
        auto MatInstance = Decal->CreateDynamicMaterialInstance();
        MatInstance->SetVectorParameterValue("Color", randColor);
        MatInstance->SetScalarParameterValue("Frame", frameNum);

        // Check if the hit object is a Perlin procedural terrain and modify it accordingly
        APerlinProcTerrain* procTerrain = Cast<APerlinProcTerrain>(OtherActor);
        if (procTerrain)
        {
            procTerrain->AlterMesh(Hit.ImpactPoint);
        }
    }
}
