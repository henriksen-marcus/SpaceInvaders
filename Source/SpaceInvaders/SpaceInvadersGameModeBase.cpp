// Copyright Epic Games, Inc. All Rights Reserved.


#include "SpaceInvadersGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerShip.h"


ASpaceInvadersGameModeBase::ASpaceInvadersGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnDistance = 12000.f;
	WaitTime = 0.f;
	Kills = 0;
	KillsToWin = 20;
	MaxAllowedEnemies = 5;
	bGameStarted = true;
}


void ASpaceInvadersGameModeBase::BeginPlay()
{
	/** Get a pointer to the HUD and make it so the UI works */
	HUDContainer = Cast<AHUDContainer>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (HUDContainer)
	{
		GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeUIOnly());
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	}
	
	/** Get a pointer to the player */
	//APlayerShip* TempShip = Cast<APlayerShip>(GetWorld()->GetFirstPlayerController());
	AActor* TempActor = UGameplayStatics::GetActorOfClass(GetWorld(), PlayerShipBP);
	if (TempActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Got TempShip!"))
		PlayerShip = Cast<APlayerShip>(TempActor);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Could not get TempShip!"))
	}
	
}


void ASpaceInvadersGameModeBase::Tick(float DeltaSeconds)
{
	if (!bGameStarted) { return; }

	/** Spawn enemies and set their target location to the player location */
	if (WaitTime >= 0.4f)
	{
		WaitTime = 0.f;
		if (PlayerShip)
		{
			PlayerLocation = PlayerShip->GetLoc();

			for (int i{}; i < SpawnedZlorps.Num(); i++) 
			{
				if (SpawnedZlorps[i])
				{
					SpawnedZlorps[i]->TargetVector = PlayerLocation;
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("Not Valid! Number of arr elem: %d"), SpawnedZlorps.Num())
				}
			}
		}
		/** Enemies are not allowed to spawn if the max amount has been reached, or the amount of kills the player has left */
		if (SpawnedZlorps.Num() < MaxAllowedEnemies && SpawnedZlorps.Num() <= 19 - Kills)
		{
			AEnemyZlorp* TempEnemy = GetWorld()->SpawnActor<AEnemyZlorp>(EnemyZlorpBP, GetRandomSpawnLocation(), FRotator::ZeroRotator);
			SpawnedZlorps.Add(TempEnemy);
		}
	}
	WaitTime += DeltaSeconds;

	if (HUDContainer)
	{
		HUDContainer->UpdateIGWidget(Kills);
	}
}


/** Use trigonometry to get a random point on a circle around the player */
FVector ASpaceInvadersGameModeBase::GetRandomSpawnLocation()
{
	float Theta = FMath::RandRange(0.f, 6.28318530718f);
	FVector RandomCircle = FVector::ZeroVector;
	RandomCircle.X = SpawnDistance * FMath::Cos(Theta);
	RandomCircle.Y = SpawnDistance * FMath::Sin(Theta);
	RandomCircle += PlayerLocation;
	RandomCircle.Z = 284.f; // Ground height
	return RandomCircle;
}


void ASpaceInvadersGameModeBase::AddKills()
{
	if (++Kills >= 20)
	{
		PlayerShip->GameWon = true;
	}
}
