// Fill out your copyright notice in the Description page of Project Settings.



#include "Components/SphereComponent.h"
#include "CombatComponent.h"
#include "Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "NewFPSGameDemoCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include <Net/UnrealNetwork.h>
#include "NewFPSGameDemoPlayerController.h"
#include "HUD/FPSHUD.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"



UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}


void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	//武器赋值
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket= Character->GetMesh()->GetSocketByName(FName("HandGrip_R"));
	UE_LOG(LogTemp, Warning, TEXT("HandSocket Valid:%d"),HandSocket);
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());//手持武器
	}
	EquippedWeapon->SetOwner(Character);//自动进行复制
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
	}
	
}

void UCombatComponent::SetAiming(bool IsAiming)
{
	bAiming = IsAiming;
	ServerSetAiming(bAiming);
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if (bFireButtonPressed)
	{
		/*FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);*/
		Fire_ButtonPressed();
	}
}

void UCombatComponent::Fire_ButtonPressed()
{
		ServerSetFiring(bFireButtonPressed, HitTarget);

		if (EquippedWeapon)
		{
			CrosshairShootingFactor = 0.75f;
		}
		StartFireTimer();

}

void UCombatComponent::ServerSetFiring_Implementation(bool bPressed,const  FVector_NetQuantize& TraceHitTarget)
{
	//服务端调用多播时运行在服务端，且也运行在客户端
	MulticastFire(TraceHitTarget);
}


void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	EquippedWeapon->Fire(TraceHitTarget);
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	//获取中心点
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),CrosshairLocation, CrosshairWorldPosition,CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;
		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;
		GetWorld()->LineTraceSingleByChannel(TraceHitResult,Start,End,ECollisionChannel::ECC_Visibility);

	}
	
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;

	Controller = Controller == nullptr ? Cast<ANewFPSGameDemoPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<AFPSHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			FHUDPackage HUDPackage;
			if (EquippedWeapon)
			{
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
			}

			// 计算准心散布

		// [0, 600] -> [0, 1]


			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			if (bAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);

			}
			CrosshairShootingFactor=FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 40.f);
			HUDPackage.CrosshairSpread =
				0.75f +
				CrosshairVelocityFactor +
				CrosshairInAirFactor -
				CrosshairAimFactor +
				CrosshairShootingFactor;

			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomedInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr||Character==nullptr) return;
	Character->GetWorldTimerManager().SetTimer(FireTimer, this, &UCombatComponent::FireTimerFinished, EquippedWeapon->FireDelay);
}

void UCombatComponent::FireTimerFinished()
{
	if (EquippedWeapon == nullptr) return;

	if (bFireButtonPressed&&EquippedWeapon->bAutomatic)
	{
		Fire_ButtonPressed();
	}
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled()) 
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
}

	