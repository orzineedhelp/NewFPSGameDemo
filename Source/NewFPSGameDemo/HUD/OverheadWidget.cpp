// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TextBlock.h"
#include "OverheadWidget.h"
#include "CharacterOverlay.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	ENetRole LocalRole = InPawn->GetLocalRole();// 当前机器上的角色
	ENetRole RemoteRole = InPawn->GetRemoteRole(); // 其他机器看到的角色

	//将枚举转换为可读字符串
	FString LRole, RRole;
	switch (LocalRole)
	{
	case ENetRole::ROLE_Authority:
		LRole = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		LRole = FString("AutonomousProxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		LRole = FString("SimulatedProxy");
		break;
	case ENetRole::ROLE_None:
		LRole = FString("None");
		break;
	}
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		RRole = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		RRole = FString("AutonomousProxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		RRole = FString("SimulatedProxy");
		break;
	case ENetRole::ROLE_None:
		RRole = FString("None");
		break;
	}

	//设置到UI显示
	FString LocalRoleString = FString::Printf(TEXT("Local Role: %s\nRemote Role:%s"), *LRole, *RRole);
	SetDisplayText(LocalRoleString);
}



void UOverheadWidget::NativeDestruct()
{
	Super::NativeDestruct();
}
