// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIGuard.generated.h"

class ATargetPoint;
class UPawnSensingComponent;

UENUM(BluePrintType)
enum class EAIState : uint8
{
	Idle,
	Patrolling,
	Suspicious,
	Alerted
};

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
	void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);

	UFUNCTION()
	void ResetOrientation();


	void SetGuardState(EAIState NewState);

	UFUNCTION()
	void OnRep_GuardState();

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnStateChanged(EAIState NewState);

	// State Enter functions
	UFUNCTION()
	void OnIdleEnter();
	UFUNCTION()
	void OnPatrolEnter();
	UFUNCTION()
	void OnSuspiciousEnter();
	UFUNCTION()
	void OnAlertedEnter();

	// State Tick functions
	UFUNCTION()
	void PatrolTick(float DeltaTime);
	UFUNCTION()
	void SuspiciousTick(float DeltaTime);
	UFUNCTION()
	void AlertedTick(float DeltaTime);

	// State Exit functions
	UFUNCTION()
	void OnPatrolExit();
	UFUNCTION()
	void OnAlertedExit();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp = nullptr;

	FRotator OriginalRotation;
	FTimerHandle TimerHandle_RestoreOrientation;

	UPROPERTY(ReplicatedUsing=OnRep_GuardState)
	EAIState GuardState = EAIState::Idle;


	UPROPERTY(EditInstanceOnly, Category = "AI")
	TArray<ATargetPoint*> TargetPoints;

	// Idle variables
	UPROPERTY(EditInstanceOnly, Category = "AI")
	float IdleDuration = 3.0f;

	// Patrol variables
	ATargetPoint* CurrentTarget = nullptr;
	int32 CurrentIndex = 0;
	bool Forward = true;

	// Suspicious variable
	FVector NoiseLocation;

	// Alerted variables
	APawn* FollowingPawn = nullptr;
	UPROPERTY(EditInstanceOnly, Category = "AI")
	float DesistDistance = 2000.0f;
};
