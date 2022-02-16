// Copyright Epic Games, Inc. All Rights Reserved.


#include "SpaceInvadersGameModeBase.h"
#include "Kismet/GameplayStatics.h"


ASpaceInvadersGameModeBase::ASpaceInvadersGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnDistance = 12000.f;
	WaitTime = 0.f;
	Kills = 0;
	KillsToWin = 20;
	MaxAllowedEnemies = 5;
}

void ASpaceInvadersGameModeBase::BeginPlay()
{
	if (GetWorld())
	{
		AActor* TempActor = UGameplayStatics::GetActorOfClass(GetWorld(), PlayerShipBP);
		APlayerShip* TempShip = Cast<APlayerShip>(TempActor);
		if (TempShip) 
		{
			PlayerLocation = TempShip->GetLoc();
			for (int i{}; i < MaxAllowedEnemies; i++) 
			{
				AEnemyZlorp* TempEnemy = GetWorld()->SpawnActor<AEnemyZlorp>(EnemyZlorpBP, GetRandomSpawnLocation(), FRotator::ZeroRotator);
				SpawnedZlorps.Add(TempEnemy);
			}
		}
	}

	HUDContainer = Cast<AHUDContainer>(GetWorld()->GetFirstPlayerController()->GetHUD());
}


void ASpaceInvadersGameModeBase::Tick(float DeltaSeconds)
{
	if (WaitTime >= 0.5f)
	{
		WaitTime = 0.f;
		AActor* TempActor = UGameplayStatics::GetActorOfClass(GetWorld(), PlayerShipBP);
		APlayerShip* TempShip = Cast<APlayerShip>(TempActor);
		if (TempShip)
		{
			PlayerLocation = TempShip->GetLoc();

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

		if (SpawnedZlorps.Num() < MaxAllowedEnemies)
		{
			AEnemyZlorp* TempEnemy = GetWorld()->SpawnActor<AEnemyZlorp>(EnemyZlorpBP, GetRandomSpawnLocation(), FRotator::ZeroRotator);
			SpawnedZlorps.Add(TempEnemy);
		}
		UE_LOG(LogTemp, Warning, TEXT("Zlorp: %d"), SpawnedZlorps.Num())
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
	RandomCircle.Z = 284.f; // Ground height + character height
	return RandomCircle;
}


void ASpaceInvadersGameModeBase::AddKills()
{
	if (++Kills >= 20)
	{
		//Game win
	}
	UE_LOG(LogTemp, Warning, TEXT("Kills: %d"), Kills)


}
