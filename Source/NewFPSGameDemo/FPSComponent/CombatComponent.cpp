// Fill out your copyright notice in the Description page of Project Settings.



#include "Components/SphereComponent.h"
#include "CombatComponent.h"
#include "Weapon/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "NewFPSGameDemoCharacter.h"
#include <Net/UnrealNetwork.h>



UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	
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
	ServerSetFiring(bFireButtonPressed);
}

void UCombatComponent::ServerSetFiring_Implementation(bool bPressed)
{
	bFireButtonPressed = bPressed;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);

}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

