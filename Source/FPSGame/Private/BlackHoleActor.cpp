// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackHoleActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ABlackHoleActor::ABlackHoleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	InnerSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("InnerSphereComp"));
	InnerSphereComp->SetSphereRadius(100);
	InnerSphereComp->SetupAttachment(MeshComp);
	// Bind to Event
	InnerSphereComp->OnComponentBeginOverlap.AddDynamic(this, &ABlackHoleActor::OverlapInnerSphere);

	OuterSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("OuterSphereComp"));
	OuterSphereComp->SetSphereRadius(3000);
	OuterSphereComp->SetupAttachment(MeshComp);
}

void ABlackHoleActor::OverlapInnerSphere(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr)
		OtherActor->Destroy();
}

// Called every frame
void ABlackHoleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<UPrimitiveComponent*> OverlapingComponents;
	OuterSphereComp->GetOverlappingComponents(OverlapingComponents);

	for (int32 i = 0; i < OverlapingComponents.Num(); ++i)
	{
		UPrimitiveComponent* Component = OverlapingComponents[i];

		if (Component != nullptr && Component->IsSimulatingPhysics())
		{
			const float radius = OuterSphereComp->GetScaledSphereRadius();
			Component->AddRadialForce(GetActorLocation(), radius, -Force, ERadialImpulseFalloff::RIF_Constant, true);
		}
	}
}