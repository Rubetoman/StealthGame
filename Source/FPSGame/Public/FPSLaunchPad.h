// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSLaunchPad.generated.h"

class UBoxComponent;

UCLASS()
class FPSGAME_API AFPSLaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSLaunchPad();

protected:

	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* OverlapComp = nullptr;

	// Impulse added to the character on overlap
	UPROPERTY(EditInstanceOnly, Category = "LaunchPad")
	float LaunchStrength = 1500.0f;

	// Angle added on top of actor rotation to launch the character
	UPROPERTY(EditInstanceOnly, Category = "LaunchPad")
	float LaunchPitchAngle = 35.0f;

	// Effect to play when activating launch pad
	UPROPERTY(EditDefaultsOnly, Category = "LaunchPad")
	UParticleSystem* ActivayeLaunchPadEffect = nullptr;
};
