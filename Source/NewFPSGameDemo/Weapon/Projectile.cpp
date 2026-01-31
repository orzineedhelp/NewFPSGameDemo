// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	/* 设置碰撞盒的碰撞通道类型为 WorldDynamic
	 ECC_WorldDynamic: 用于动态移动的物体，如可移动的Actor、物理模拟的物体等*/
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	/*启用碰撞盒的碰撞检测功能
	 QueryAndPhysics: 同时启用查询碰撞（如射线检测）和物理碰撞（如物体阻挡）*/
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	/*首先将所有碰撞通道的响应设置为忽略（初始化）
	 ECR_Ignore: 完全忽略该通道的碰撞，既不阻挡也不响应*/
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	/*单独设置对 Visibility 通道的碰撞响应为阻挡（Block）
	 ECC_Visibility: 用于射线检测（如瞄准、光标选择）
	 ECR_Block: 阻挡射线通过，射线会命中并返回命中结果
	 这意味着这个碰撞盒可以被射线检测到，适用于瞄准或光标选择*/
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	/*单独设置对 WorldStatic 通道的碰撞响应为阻挡（Block）
	 ECC_WorldStatic: 用于静态的、不移动的世界几何体（如墙壁、地面）
	 ECR_Block: 阻挡物理移动，防止物体穿过
	 这意味着这个碰撞盒会与静态世界几何体发生物理碰撞，防止穿透*/
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	//子弹移动
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

void AProjectile::Destroyed()
{
	Super::Destroyed();
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());

	}
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (Tracer)
	{
		TracerComponent = UGameplayStatics::SpawnEmitterAttached(Tracer, CollisionBox, FName(), GetActorLocation(), GetActorRotation(),
			EAttachLocation::KeepWorldPosition);
	}
	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}

}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

