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

	UFUNCTION(Server, Reliable)
	void ServerSetFiring(bool bPressed,const FVector_NetQuantize& TraceHitTarget );

	//¶à²¥
	UFUNCTION(NetMulticast,Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);


public:	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	class ANewFPSGameDemoCharacter* Character;

	UPROPERTY(Replicated)
	class AWeapon* EquippedWeapon;

	bool bAiming;
	bool bFireButtonPressed;


};
