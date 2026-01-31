// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Components/SphereComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),// 初始状态
	EWS_Equipped UMETA(DisplayName = "Equipped"),// 已装备（主要）
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),// 已装备（次要）
	EWS_Dropped UMETA(DisplayName = "Dropped"), // 掉落状态

	EWS_MAX UMETA(DisplayName = "DefaultMAX")// 枚举最大值，用于迭代或错误检查
};

UCLASS()
class NEWFPSGAMEDEMO_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	void ShowPickUpWidget(bool bShowWidget);
	//用于指定哪些变量需要网络复制以及它们的复制条件
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Fire(const FVector& HitTarget);


	//准线
	UPROPERTY(EditAnywhere, Category = Crosshairs)
	class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
	UTexture2D* CrosshairsBottom;


protected:
	virtual void BeginPlay() override;

	/// <summary>
	/// 开始重叠
	/// </summary>
	/// <param name="OverlappedComponent">产生重叠事件的组件（AreaSphere）</param>
	/// <param name="OtherActor">与之重叠的Actor（玩家）</param>
	/// <param name="OtherComp">重叠Actor上带有的组件</param>
	/// <param name="OtherBodyIndex">物理体索引（用于物理模拟）</param>
	/// <param name="bFromSweep">是否来自扫掠检测</param>
	/// <param name="SweepResult">扫掠检测的命中结果</param>
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/// <summary>
	/// 结束重叠
	/// </summary>
	/// <param name="OverlappedComponent">产生重叠事件的组件</param>
	/// <param name="OtherActor">与之重叠的Actor（</param>
	/// <param name="OtherComp">重叠Actor上带有的组件</param>
	/// <param name="OtherBodyIndex">物理体索引</param>
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public:	
	virtual void Tick(float DeltaTime) override;

	
	void SetWeaponState(EWeaponState State);


	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")

	class USphereComponent* AreaSphere;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(ReplicatedUsing=OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;


	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickUpWidget;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimation;

	// 音频组件
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UAudioComponent* FireAudio;

	// 粒子系统组件
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UParticleSystemComponent* MuzzleFlash;
	
	



};
