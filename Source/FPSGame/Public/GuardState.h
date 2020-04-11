// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class AFPSAIGuard;

/**
 * Custom Interface for Guards AI
 */
class FPSGAME_API IGuardState
{
public:
	IGuardState(AFPSAIGuard* Guard) : MyGuard(Guard){};
	virtual ~IGuardState() {};

	virtual void OnEnter() {};
	virtual void Tick(float DeltaTime) = 0;
	virtual void OnExit() {};

protected:
	AFPSAIGuard* MyGuard = nullptr;
};
