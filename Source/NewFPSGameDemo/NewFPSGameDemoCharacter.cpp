// Copyright Epic Games, Inc. All Rights Reserved.


#include "Components/WidgetComponent.h"
#include "NewFPSGameDemoCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "NewFPSGameDemo.h"

ANewFPSGameDemoCharacter::ANewFPSGameDemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	// 初始化冲刺
	bIsSprinting = false;
	SprintSpeedMultiplier = 1.5f;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);
}

void ANewFPSGameDemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ANewFPSGameDemoCharacter::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ANewFPSGameDemoCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANewFPSGameDemoCharacter::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANewFPSGameDemoCharacter::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ANewFPSGameDemoCharacter::LookInput);

		//Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ANewFPSGameDemoCharacter::DoSprintStart);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ANewFPSGameDemoCharacter::DoSprintEnd);

		// Crouch
		EnhancedInputComponent->BindAction(DownAction, ETriggerEvent::Triggered, this, &ANewFPSGameDemoCharacter::DoDown);

	}
	else
	{
		UE_LOG(LogNewFPSGameDemo, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANewFPSGameDemoCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 注册 OverlappingWeapon 进行网络复制
	DOREPLIFETIME(ANewFPSGameDemoCharacter, OverlappingWeapon);
}



void ANewFPSGameDemoCharacter::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();
	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void ANewFPSGameDemoCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void ANewFPSGameDemoCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ANewFPSGameDemoCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
			// pass the move inputs
			AddMovementInput(GetActorRightVector(), Right);
			AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void ANewFPSGameDemoCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void ANewFPSGameDemoCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void ANewFPSGameDemoCharacter::DoDown()
{	//使用Character自带的蹲下

	if (IsCrouched())  
	{
		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Crouch Ended"));
		}*/
		UnCrouch();    
	}
	else             
	{
		/*if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("CanCrouch:%d "),CanCrouch()));
		}*/
		if (CanCrouch()) // 检查是否可以蹲下
		{
			Crouch();   

		}
	}
}

void ANewFPSGameDemoCharacter::DoSprintStart()
{
	if (!GetCharacterMovement() || bIsSprinting) return;

	// 检查是否可以冲刺
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		bIsSprinting = true;

		// 蹲下
		if (IsCrouched())
		{
			// 记录当前蹲下速度
			if (DefaultWalkSpeedCrouched == 0.0f)
			{
				DefaultWalkSpeedCrouched = GetCharacterMovement()->MaxWalkSpeedCrouched;
			}
			// 设置冲刺蹲下速度
			GetCharacterMovement()->MaxWalkSpeedCrouched = DefaultWalkSpeedCrouched * SprintSpeedMultiplier;
		}
		else//正常行走
		{
			// 记录当前行走速度
			if (DefaultWalkSpeed == 0.0f)
			{
				DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
			}
			// 设置冲刺行走速度
			GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed * SprintSpeedMultiplier;
		}
	}
}

void ANewFPSGameDemoCharacter::DoSprintEnd()
{

	if (!bIsSprinting) return;

	bIsSprinting = false;

	if (IsCrouched())
	{// 恢复为正常蹲下速度
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeedCrouched = DefaultWalkSpeedCrouched;
		}

	}
	else
	{		// 恢复为正常行走速度
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
		}

	}
}

