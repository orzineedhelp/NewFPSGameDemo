// Fill out your copyright notice in the Description page of Project Settings.

#include "NewFPSGameDemoCharacter.h"
#include "FPSAnimInstance.h"

void UFPSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	FPSCharacter = Cast<ANewFPSGameDemoCharacter>(TryGetPawnOwner());
}

void UFPSAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (FPSCharacter == nullptr)
	{
		FPSCharacter = Cast<ANewFPSGameDemoCharacter>(TryGetPawnOwner());

	}
	if (FPSCharacter == nullptr) return;

	bWeaponEquipped = FPSCharacter->IsWeaponEquipped();
	bAiming = FPSCharacter->IsAiming();
	bFiring = FPSCharacter->IsFiring();
}
