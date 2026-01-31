// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "NewFPSGameDemoCharacter.h"
#include "Components/WidgetComponent.h"
#include <Net/UnrealNetwork.h>
#include "Animation/AnimationAsset.h"
#include "Components/AudioComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;// 启用网络复制

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh);

	// 设置武器网格的碰撞
	// 默认对所有通道为阻挡（Block），但对Pawn通道忽略（Ignore），以免与玩家发生碰撞
	// 然后禁用碰撞，这样武器不会与任何物体发生碰撞，但后续可以根据状态重新启用
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	AreaSphere= CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	// 初始时禁用碰撞，等待在BeginPlay中根据权限启用
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(RootComponent);

	// 创建并配置音频组件
	FireAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("FireAudio"));
	FireAudio->SetupAttachment(RootComponent);
	FireAudio->bAutoActivate = false;  // 不自动播放
	FireAudio->SetRelativeLocation(FVector(0, 0, 0));

	// 创建并配置粒子系统组件
	MuzzleFlash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("MuzzleFlash"));
	MuzzleFlash->SetupAttachment(GetWeaponMesh(), FName("Muzzle"));  // 附加到枪口插槽
	MuzzleFlash->bAutoActivate = false;  // 不自动播放
	MuzzleFlash->SetRelativeLocation(FVector(0, 0, 0));
}

void AWeapon::ShowPickUpWidget(bool bShowWidget)
{
	if (PickUpWidget) 
	{
		PickUpWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, WeaponState);

}

void AWeapon::Fire(const FVector& HitTarget)
{
	if(FireAudio) FireAudio->Play();
	if (MuzzleFlash)
	{
		const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("Muzzle"));
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		// 确保组件在正确位置
		MuzzleFlash->SetWorldTransform(SocketTransform);
		// 播放粒子效果
		MuzzleFlash->Activate();
	}

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (PickUpWidget)
	{
		PickUpWidget->SetVisibility(false);

	}
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ANewFPSGameDemoCharacter * FPSCharacter = Cast<ANewFPSGameDemoCharacter>(OtherActor);
	if (FPSCharacter)
	{
		FPSCharacter->SetOverlappingWeapon(this);
		
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ANewFPSGameDemoCharacter* FPSCharacter = Cast<ANewFPSGameDemoCharacter>(OtherActor);
	if (FPSCharacter)
	{
		FPSCharacter->SetOverlappingWeapon(nullptr);

	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	
	switch (WeaponState)
	{

	case EWeaponState::EWS_Equipped:
		ShowPickUpWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}

}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:
		break;
	case EWeaponState::EWS_Equipped:
		ShowPickUpWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_EquippedSecondary:
		break;
	case EWeaponState::EWS_Dropped:
		break;
	case EWeaponState::EWS_MAX:
		break;
	default:
		break;
	}
}

