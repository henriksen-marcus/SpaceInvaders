// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyZlorp.h"
#include "HealthPack.h"
#include "Kismet/KismetMathLibrary.h"
#include "SpaceInvadersGameModeBase.h"

AEnemyZlorp::AEnemyZlorp()
{
	PrimaryActorTick.bCanEverTick = true;
	TargetVector = FVector::ZeroVector;
	Health = 200.f;
	ZlorpSpeed = 3800.f;
	TargetOffset = 0;
	Timer = 0;

	InitialMaterial0 = GetMesh()->GetMaterial(0);
	InitialMaterial1 = GetMesh()->GetMaterial(1);
}


void AEnemyZlorp::BeginPlay()
{
	Super::BeginPlay();
}


void AEnemyZlorp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetVector);
	TargetRotation.Yaw += TargetOffset;
	TargetRotation.Pitch = 0.f;
	
	float DistanceToTarget = UKismetMathLibrary::Vector_Distance(GetActorLocation(), TargetVector);

	if (DistanceToTarget > 50000.f)
	{
		/** Incase an enemy spawns outside the map or something, they will just respawn */
		KillSelf(false);
	}
	else if (DistanceToTarget > 500.f)
	{
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 3.f));
		AddActorLocalOffset(FVector(ZlorpSpeed * DeltaTime, 0.f, 0.f), true);
	}
	else if (DistanceToTarget > 200.f) {
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10.f));
		AddActorLocalOffset(FVector(ZlorpSpeed * DeltaTime * 0.4f, 0.f, 0.f), true);
	}

	if (Timer >= 5.f)
	{
		Timer = 0;
		TargetOffset = FMath::RandRange(-20.f, 20.f);
	}
	Timer += DeltaTime;
}


void AEnemyZlorp::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


float AEnemyZlorp::GetHealth()
{
	return Health;
}


void AEnemyZlorp::AddHealth(float ChangeAmount)
{
	Health = FMath::Clamp(Health + ChangeAmount, 0.f, 200.f);
	
	/** If the enemy takes damage, play the damage effect */
	if (ChangeAmount < 0 && DamageMaterial)
	{
		GetMesh()->SetMaterial(0, DamageMaterial);
		GetMesh()->SetMaterial(1, DamageMaterial);

		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &AEnemyZlorp::EndDamageEffect, 0.1f, false);
	}

	if (!Health)
	{
		if (DeathFX)
		{
			FVector Loc = GetActorLocation();
			Loc.Z -= 70.f;
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathFX, Loc, GetActorRotation(), FVector(2.f));
		}
		KillSelf(true);
	}
}


void AEnemyZlorp::EndDamageEffect()
{
	/** Here we need to use GetMaterial() since the mesh component on the Character class is private */
	GetMesh()->SetMaterial(0, InitialMaterial0);
	GetMesh()->SetMaterial(1, InitialMaterial1);
}


/** Manage garbage and add kills to the counter in the custom gamemode */
void AEnemyZlorp::KillSelf(bool PlayerKill)
{
	ASpaceInvadersGameModeBase* GameMode = Cast<ASpaceInvadersGameModeBase>(GetWorld()->GetAuthGameMode());
	TArray<AEnemyZlorp*> SpawnedZlropsArr = GameMode->SpawnedZlorps;
	GameMode->SpawnedZlorps.Remove(this);
	if (PlayerKill)
	{
		GameMode->AddKills();
	}


	//Spawn Helath Pack
	int random = 1 + (rand() % 100);

	if (random <= 100) //25% chance of dropping a HealthPack
	{
		//Spawn - HealthPack
		UE_LOG(LogTemp, Warning, TEXT("EnemyZlorp - Spawn HealthPack"));
		UWorld* World = GetWorld();
		AHealthPack* TempHealthPack = World->SpawnActor<AHealthPack>(HealthPack, GetActorLocation(), GetActorRotation());
	}

	this->Destroy();
}

