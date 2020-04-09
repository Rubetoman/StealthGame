// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSLaunchPad.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSLaunchPad::AFPSLaunchPad()
{
	OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComp"));
	OverlapComp->SetBoxExtent(FVector(75.0f, 75.0f, 50.0f));
	RootComponent = OverlapComp;

	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSLaunchPad::HandleOverlap);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
}

void AFPSLaunchPad::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Calculate Velocity vector with desired pitch and strength
	FRotator LaunchDirection = GetActorRotation();
	LaunchDirection.Pitch += LaunchPitchAngle;
	FVector LaunchVelocity = LaunchDirection.Vector() * LaunchStrength;

	ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);
	if (OtherCharacter != nullptr)
	{
		// Launch Character
		OtherCharacter->LaunchCharacter(LaunchVelocity, true, true);

		// Spawn effect
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActivayeLaunchPadEffect, GetActorLocation());
	}
	else if (OtherComp != nullptr && OtherComp->IsSimulatingPhysics())
	{
		// Add Impulse to component simulating physics
		OtherComp->AddImpulse(LaunchVelocity, NAME_None, true);

		// Spawn effect
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActivayeLaunchPadEffect, GetActorLocation());
	}
}