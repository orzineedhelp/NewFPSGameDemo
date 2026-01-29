// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "NewFPSGameDemoCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

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
}

void AWeapon::ShowPickUpWidget(bool bShowWidget)
{
	if (PickUpWidget) 
	{
		PickUpWidget->SetVisibility(bShowWidget);
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
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

