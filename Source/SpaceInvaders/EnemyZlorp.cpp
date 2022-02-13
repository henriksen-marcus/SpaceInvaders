// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyZlorp.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemyZlorp::AEnemyZlorp()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TargetVector = FVector::ZeroVector;
	Health = 200.f;
	ZlorpSpeed = 1700.f;
}


// Called when the game starts or when spawned
void AEnemyZlorp::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void AEnemyZlorp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetVector);
	TargetRotation.Pitch = 0.f;
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 2.f));
	AddActorLocalOffset(FVector(ZlorpSpeed * DeltaTime, 0.f, 0.f));
}


// Called to bind functionality to input
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

	if (ChangeAmount < 0 && HitSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), HitSound);
	}

	if (!Health)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s died!"), *this->GetFName().ToString());
		this->Destroy();
	}
}

