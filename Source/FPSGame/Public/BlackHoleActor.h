// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlackHoleActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class FPSGAME_API ABlackHoleActor : public AActor
{
	GENERATED_BODY()	

public:	
	// Sets default values for this actor's properties
	ABlackHoleActor();

	UFUNCTION()
	void OverlapInnerSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp = nullptr;

	// Inner sphere that destroys overlapping components
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* InnerSphereComp = nullptr;

	// Outer sphere that attracts overlapping components (If physics symulated)
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* OuterSphereComp = nullptr;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	float Force = 2000.0f;
};
