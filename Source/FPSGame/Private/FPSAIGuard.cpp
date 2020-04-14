// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "FPSGameMode.h"
#include "Engine/TargetPoint.h"
#include "Transform.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	
	OriginalRotation = GetActorRotation();
	OnIdleEnter();
}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr) return;

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f);

	FollowingPawn = SeenPawn;

	SetGuardState(EAIState::Alerted);
}

void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alerted) return;

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);

	// Make guard look at noise position
	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;

	SetActorRotation(NewLookAt);

	// Reset rotation after 3 seconds
	GetWorldTimerManager().ClearTimer(TimerHandle_RestoreOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_RestoreOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);

	SetGuardState(EAIState::Suspicious);
}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted) return;

	SetActorRotation(OriginalRotation);
	SetGuardState(EAIState::Idle);
}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState) return;

	switch (GuardState)
	{
	case EAIState::Patrolling:	OnPatrolExit();		break;
	case EAIState::Alerted:		OnAlertedExit();	break;
	case EAIState::Idle:
	case EAIState::Suspicious:
	default:
		break;
	}

	GuardState = NewState;

	switch (GuardState)
	{
	case EAIState::Idle:		OnIdleEnter();		break;
	case EAIState::Patrolling:	OnPatrolEnter();	break;
	case EAIState::Alerted:		OnAlertedEnter();	break;
	case EAIState::Suspicious:
	default:
		break;
	}

	OnStateChanged(NewState);
}

void AFPSAIGuard::OnIdleEnter()
{
	// Start patrolling after 3 seconds
	FTimerHandle TimerHandle_StartPatrolling;
	FTimerDelegate PatrolDelegate = FTimerDelegate::CreateUObject(this, &AFPSAIGuard::SetGuardState, EAIState::Patrolling);
	GetWorldTimerManager().SetTimer(TimerHandle_StartPatrolling, PatrolDelegate, 1.0f, false);
}

void AFPSAIGuard::OnPatrolEnter()
{
	// Get next point to walk to
	if (Forward)
	{
		if (CurrentIndex < TargetPoints.Num() - 1)
		{
			++CurrentIndex;
		}
		else
		{
			Forward = !Forward;
			--CurrentIndex;
		}
	}
	else
	{
		if (CurrentIndex > 0)
		{
			--CurrentIndex;
		}
		else
		{
			Forward = !Forward;
			++CurrentIndex;
		}
	}
	CurrentTarget = TargetPoints[CurrentIndex];
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), CurrentTarget);
}

void AFPSAIGuard::OnAlertedEnter()
{
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), FollowingPawn);
}

void AFPSAIGuard::PatrolTick(float DeltaTime)
{
	if (!CurrentTarget) return;

	// Check distance to target
	FVector Delta = GetActorLocation() - CurrentTarget->GetActorLocation();
	float Distance = Delta.Size();

	if (Distance < 50)
	{
		// Change to idle onced the target has been reached
		SetGuardState(EAIState::Idle);
	}	
}

void AFPSAIGuard::AlertedTick(float DeltaTime)
{
	// Check distance to target
	FVector Delta = GetActorLocation() - FollowingPawn->GetActorLocation();
	float Distance = Delta.Size();

	if (Distance < 100)
	{
		// Game over
		AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
		if (GM != nullptr && FollowingPawn != nullptr)
			GM->CompleteMission(FollowingPawn, false);
	}
	else if (Distance > 2000)
	{
		// Pawn has escaped, return to patrol
		SetGuardState(EAIState::Patrolling);
	}
}

void AFPSAIGuard::OnPatrolExit()
{
	AController* Controller = GetController();
	if (Controller != nullptr)
	{
		Controller->StopMovement();
	}
}

void AFPSAIGuard::OnAlertedExit()
{
	AController* Controller = GetController();
	if (Controller != nullptr)
	{
		Controller->StopMovement();
	}
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GuardState == EAIState::Patrolling)
		PatrolTick(DeltaTime);
	else if (GuardState == EAIState::Alerted)
		AlertedTick(DeltaTime);
}