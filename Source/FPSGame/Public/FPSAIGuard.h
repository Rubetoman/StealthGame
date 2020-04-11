// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/Interface.h"
#include "GuardState.h"
#include "FPSAIGuard.generated.h"

class UPawnSensingComponent;

class FPSGAME_API UIdleState : public IGuardState
{
public:
	UIdleState(AFPSAIGuard* Guard) : IGuardState(Guard) {};

	void Tick(float DeltaTime) override;

protected:
	float Timer = 0.0f;
};

class FPSGAME_API UPatrolState : public IGuardState
{
public:
	UPatrolState(AFPSAIGuard* Guard) : IGuardState(Guard) {};

	void Tick(float DeltaTime) override;

	FVector NextTarget;
};

class FPSGAME_API USuspiciousState : public IGuardState
{
public:
	USuspiciousState(AFPSAIGuard* Guard) : IGuardState(Guard) {};

	void Tick(float DeltaTime) override;

	FVector Target;
};

class FPSGAME_API UAlertedState : public IGuardState
{
public:
	UAlertedState(AFPSAIGuard* Guard) : IGuardState(Guard) {};

	void Tick(float DeltaTime) override;
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

	//UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	//void OnStateChanged(IGuardState* NewState);
	//void OnStateChanged(EAIState NewState);


public:	
	void SetGuardState(IGuardState* NewState);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	IGuardState* CurrentState = nullptr;

	UIdleState* Idle = nullptr;
	UPatrolState* Patrol = nullptr;
	USuspiciousState* Suspicious = nullptr;
	UAlertedState* Alerted = nullptr;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp = nullptr;

	FRotator OriginalRotation;
	FTimerHandle TimerHandle_RestoreOrientation;
};
