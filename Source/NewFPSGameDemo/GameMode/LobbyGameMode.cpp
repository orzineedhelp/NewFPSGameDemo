// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "LobbyGameMode.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (GameState)
	{
		int32 NumberOfPlayers = GetNumPlayers();
		
		if (NumberOfPlayers == 2)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				bUseSeamlessTravel = true;//设置无缝旅行
				World->ServerTravel(FString("/Game/Maps/GameMap?listen"));//打开指定地图并且设置服务端
			}
		}

	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>();

	if (GameState)
	{
		if (GEngine)
		{
			if (PlayerState)
			{
				int32 NumberOfPlayers = GetNumPlayers();
				GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Yellow, FString::Printf(TEXT("Player Number:%d"), NumberOfPlayers - 1));

				FString PlayerName = PlayerState->GetPlayerName();
				GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Cyan, FString::Printf(TEXT("Player %s has exited"), *PlayerName));

			}

		}
	}
}
