// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

#define  TRACE_LENGTH 80000.f

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEWFPSGAMEDEMO_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	friend class ANewFPSGameDemoCharacter;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	void EquipWeapon(class AWeapon* WeaponToEquip);
protected:
	virtual void BeginPlay() override;
	void SetAiming(bool IsAiming);

	UFUNCTION(Server,Reliable)
	void ServerSetAiming(bool bIsAiming);


	void FireButtonPressed(bool bPressed);

	void Fire_ButtonPressed();

	UFUNCTION(Server, Reliable)
	void ServerSetFiring(bool bPressed,const FVector_NetQuantize& TraceHitTarget );

	//多播
	UFUNCTION(NetMulticast,Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

public:	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	class ANewFPSGameDemoCharacter* Character;
	class ANewFPSGameDemoPlayerController* Controller;
	class AFPSHUD* HUD;

	UPROPERTY(Replicated)
	class AWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed;

	bool bAiming;
	bool bFireButtonPressed;
	bool bHit;

	//HUD准线
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	FVector HitTarget;

	//Aiming FOV
	float DefaultFOV;
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	//自动开火计时器
	FTimerHandle FireTimer;



	void StartFireTimer();
	//回调函数控制时间
	void FireTimerFinished();
public:
};
