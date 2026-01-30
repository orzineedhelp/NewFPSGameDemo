// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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

	
	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; };

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")

	class USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;


	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickUpWidget;
	
};
