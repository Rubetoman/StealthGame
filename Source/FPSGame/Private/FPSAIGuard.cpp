// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "FPSGameMode.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	Idle = new UIdleState(this);
	Patrol = new UPatrolState(this);
	Suspicious = new USuspiciousState(this);
	Alerted = new UAlertedState(this);

	CurrentState = Idle;
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	
	OriginalRotation = GetActorRotation();
}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr) return;

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f);

	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());

	if (GM != nullptr)
		GM->CompleteMission(SeenPawn, false);

	//SetGuardState(EAIState::Alerted);
	SetGuardState(Alerted);
}

void AFPSAIGuard::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	//if (GuardState == EAIState::Alerted) return;
	if (CurrentState == Alerted) return;

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);

	// Make guard look at noise position
	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt;
	NewLookAt.Yaw = FRotationMatrix::MakeFromX(Direction).Rotator().Yaw;

	SetActorRotation(NewLookAt);

	// Reset rotation after 3 seconds
	GetWorldTimerManager().ClearTimer(TimerHandle_RestoreOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_RestoreOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);

	//SetGuardState(EAIState::Suspicious);
	SetGuardState(Suspicious);
}

void AFPSAIGuard::ResetOrientation()
{
	//if (GuardState == EAIState::Alerted) return;
	if (CurrentState == Alerted) return;

	SetActorRotation(OriginalRotation);
	//SetGuardState(EAIState::Idle);
	SetGuardState(Idle);
}

/*void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState) return;

	GuardState = NewState;

	OnStateChanged(NewState);
}*/

void AFPSAIGuard::SetGuardState(IGuardState* NewState)
{
	if (CurrentState == NewState) return;

	CurrentState->OnExit();
	CurrentState = NewState;
	CurrentState->OnEnter();

	//OnStateChanged(NewState);
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void UIdleState::Tick(float DeltaTime)
{
	// After 3 seconds start moving again
	if (Timer > 3.0f)
	{
		Timer = 0.0f;
		MyGuard->SetGuardState(MyGuard->Patrol);
	}
	else
	{
		Timer += DeltaTime;
	}
}

void UPatrolState::Tick(float DeltaTime)
{
	// Walk toward next target
}

void USuspiciousState::Tick(float DeltaTime)
{
	// Walk towards target
}

void UAlertedState::Tick(float DeltaTime)
{
	// Run towards player
}
