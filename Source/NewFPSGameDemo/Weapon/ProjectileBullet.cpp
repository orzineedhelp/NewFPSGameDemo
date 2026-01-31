// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NewFPSGameDemoCharacter.h"
#include "NewFPSGameDemoPlayerController.h"


void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ANewFPSGameDemoCharacter* OwnerCharacter = Cast<ANewFPSGameDemoCharacter>(GetOwner());

	if (OwnerCharacter)
	{
		ANewFPSGameDemoPlayerController* OwnerController =
			Cast<ANewFPSGameDemoPlayerController>(OwnerCharacter->GetController());
		
		if (OwnerController)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());

		}
	}
	GEngine->AddOnScreenDebugMessage
	(
		-1,
		10,
		FColor::Blue,
		"Hit!!!!!"
	);


	Super::OnHit(HitComp,  OtherActor, OtherComp,  NormalImpulse,  Hit);
	
}
