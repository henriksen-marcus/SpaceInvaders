// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnemyZlorp.h"
#include "HUDContainer.h"
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

	FVector GetRandomSpawnLocation();

	void AddKills();

	// Variables
	FVector PlayerLocation;

	UPROPERTY(EditAnywhere, Category = "ActorsToSpawn")
	TSubclassOf<class AEnemyZlorp> EnemyZlorpBP;

	UPROPERTY(EditAnywhere, Category = "ActorsToSpawn")
	TSubclassOf<class APlayerShip> PlayerShipBP;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float SpawnDistance;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	int KillsToWin;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	int MaxAllowedEnemies;

	UPROPERTY()
	TArray<class AEnemyZlorp*> SpawnedZlorps;

	float WaitTime;
	int Kills;
	bool bGameStarted;

	class AHUDContainer* HUDContainer;
};
