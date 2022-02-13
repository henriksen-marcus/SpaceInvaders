// Copyright Epic Games, Inc. All Rights Reserved.


#include "SpaceInvadersGameModeBase.h"
#include "Kismet/GameplayStatics.h"


ASpaceInvadersGameModeBase::ASpaceInvadersGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	bAllDead = false;
	SpawnsRemaining = 20;
	SpawnDistance = 4000.f;
	WaitTime = 0.f;
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
			for (int i{}; i < 1; i++) 
			{
				float Theta = FMath::RandRange(0.f, 6.28318530718f);
				FVector RandomCircle = FVector(0.f);
				RandomCircle.X = SpawnDistance * FMath::Cos(Theta);
				RandomCircle.Y = SpawnDistance * FMath::Sin(Theta);
				RandomCircle += PlayerLocation;
				RandomCircle.Z = 49.5f;
				AEnemyZlorp* TempEnemy = GetWorld()->SpawnActor<AEnemyZlorp>(EnemyZlorpBP, RandomCircle, FRotator::ZeroRotator);
				SpawnedZlorps.Add(TempEnemy);
				UE_LOG(LogTemp, Warning, TEXT("Spawned a Zlorp, current amount: %d"), SpawnedZlorps.Num())
			}
		}
	}
}


void ASpaceInvadersGameModeBase::Tick(float DeltaSeconds)
{
	if (WaitTime >= 1.f)
	{
		WaitTime = 0.f;
		AActor* TempActor = UGameplayStatics::GetActorOfClass(GetWorld(), PlayerShipBP);
		APlayerShip* TempShip = Cast<APlayerShip>(TempActor);
		if (TempShip)
		{
			FVector Vec = TempShip->GetLoc();
			if (SpawnedZlorps[0])
			{
				SpawnedZlorps[0]->TargetVector = Vec;
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Not Valid! Number of arr elem: %d"), SpawnedZlorps.Num())
			}
			
		}
	}
	WaitTime += DeltaSeconds;
}
