// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "PlayerShip.h"
#include "EnemyZlorp.h"

#include "SpaceInvadersGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SPACEINVADERS_API ASpaceInvadersGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ASpaceInvadersGameModeBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// Variables
	FVector PlayerLocation;

	UPROPERTY(EditAnywhere, Category = "ActorsToSpawn")
	TSubclassOf<class AEnemyZlorp> EnemyZlorpBP;

	UPROPERTY(EditAnywhere, Category = "ActorsToSpawn")
	TSubclassOf<class APlayerShip> PlayerShipBP;

	TArray<class AEnemyZlorp*> SpawnedZlorps;

	bool bAllDead;
	float SpawnsRemaining;
	float SpawnDistance;
	float WaitTime;
};
