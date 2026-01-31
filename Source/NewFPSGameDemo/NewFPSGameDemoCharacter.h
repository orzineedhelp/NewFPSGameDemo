// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include <Weapon/Weapon.h>

#include "NewFPSGameDemoCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class ANewFPSGameDemoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

protected:
	virtual void BeginPlay() override;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input")
	class UInputAction* MouseLookAction;
	
public:
	ANewFPSGameDemoCharacter();

protected:

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

protected:

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	//用于指定哪些变量需要网络复制以及它们的复制条件
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitializeComponents() override;


protected:
	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* SprintAction;

	/** Dowm Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DownAction;

	/** Equip Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* EquipAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* AimAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* FireAction_FPS;

private:
	// 冲刺相关变量 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Sprint", meta = (AllowPrivateAccess = "true"))
	float SprintSpeedMultiplier = 1.5f;  // 冲刺速度倍数

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Sprint", meta = (AllowPrivateAccess = "true"))
	bool bIsSprinting; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Sprint", meta = (AllowPrivateAccess = "true"))
	float DefaultWalkSpeed;  // 存储默认行走速度

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Sprint", meta = (AllowPrivateAccess = "true"))
	float DefaultWalkSpeedCrouched;  // 存储默认蹲下行走速度


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement|Equip", meta = (AllowPrivateAccess = "true"))
	bool bIsEquipped;

protected:
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoDown();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoSprintStart();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoSprintEnd();

	UFUNCTION(BlueprintCallable, Category = "Input")
		virtual void DoEquip();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoAimStart();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoAimEnd();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoFireStart();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoFireEnd();


	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor,float Damage,const UDamageType* DamageType,class AController* InstigatorController,AActor* DamageCauser);

	void UpdateHUDHealth();


private:

	UPROPERTY(VisibleAnyWhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing=ONRep_OverlappingWeapon)// 复制时使用的回调函数
	class AWeapon* OverlappingWeapon;  // 指向武器对象的指针

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);// 复制回调函数


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UFUNCTION(Server,Reliable)
	void ServerEquipButtonPressed();



public:

	void SetOverlappingWeapon(AWeapon* Weapon);


	bool IsWeaponEquipped();

	bool IsAiming();

	bool IsFiring();
	
	bool IsHit;

	FVector GetHitTarget() const;

	FORCEINLINE UCameraComponent* GetFollowCamera() const {  return FirstPersonCameraComponent; }

	


private:
	//角色血条
	UPROPERTY(EditAnywhere, Category = "PlayerState")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere,Category = "PlayerState")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health();

	void Elim();

	class ANewFPSGameDemoPlayerController* PlayerController;
};

