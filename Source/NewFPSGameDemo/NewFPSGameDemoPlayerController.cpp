// Copyright Epic Games, Inc. All Rights Reserved.

#include "Components/ProgressBar.h"
#include "NewFPSGameDemoPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "NewFPSGameDemoCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "NewFPSGameDemo.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "HUD/FPSHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Components/TextBlock.h"

ANewFPSGameDemoPlayerController::ANewFPSGameDemoPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = ANewFPSGameDemoCameraManager::StaticClass();
}

void ANewFPSGameDemoPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogNewFPSGameDemo, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}

	FPSHUD = Cast<AFPSHUD>(GetHUD());
}

void ANewFPSGameDemoPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
	
}

void ANewFPSGameDemoPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	FPSHUD = FPSHUD == nullptr ? Cast<AFPSHUD>(GetHUD()) :FPSHUD;
	bool bHUDValid = FPSHUD && FPSHUD->CharacterOverlay && FPSHUD->CharacterOverlay->HealthBar && FPSHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		FPSHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		FPSHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	
}
