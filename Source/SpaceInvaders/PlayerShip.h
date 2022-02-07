// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "PlayerShip.generated.h"

UCLASS()
class SPACEINVADERS_API APlayerShip : public APawn
{
	GENERATED_BODY()
public:
	// Sets default values for this pawn's properties
	APlayerShip();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ! CUSTOM ! Hardcode key mapping
	static void InitializeDefaultPawnInputBinding();

public:

	UPROPERTY(EditDefaultsOnly, Category = "PlayerMesh")
		UStaticMeshComponent* BaseMesh = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "CameraVariables")
		USpringArmComponent* SpringArm {nullptr};

	UPROPERTY(BlueprintReadOnly, Category = "CameraVariables")
		UCameraComponent* Camera = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "EditableVariables")
		float SpeedMultiplier = 1.f;

	UPROPERTY(BlueprintReadWrite, Category = "EditableVariables")
		int Ammo = 30;

	UPROPERTY(BlueprintReadWrite, Category = "SoundVariables")
		USoundBase* ShootingSound = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "SoundVariables")
		USoundBase* ReloadingSound = nullptr;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	TSubclassOf<AActor> BulletActorToSpawn;

	bool InContact;

private:

	FVector InitialLocation = FVector::ZeroVector;

	// Player input
	void Forward(float Value);
	void Right(float Value);
	void Pitch(float Value);
	void Yaw(float Value);

	void Dash();
	void Shoot();
	void Reload();
	void ResetLocation();
	
	// Holds the amount the player is to be offset from their current location per tick
	FVector OffsetVector = FVector(0.f);
	float Force = 23600000.f;
	float DashTimer = 0.f;
};
