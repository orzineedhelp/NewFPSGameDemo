// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"
#include "ProjectileWeapon.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if (!HasAuthority()) return;
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("Muzzle"));

	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform =  MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		//目标位置减去枪口位置得来中间向量，再得到这个向量的方向作为子弹方向
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		if (ProjectileClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<AProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParams
				);
			}
		}
	}
}
