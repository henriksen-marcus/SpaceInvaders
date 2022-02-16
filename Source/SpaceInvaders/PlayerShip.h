// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "EnemyZlorp.h"
#include "SpaceInvadersGameModeBase.h"
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
	/** Returns the location of the playership */
	UFUNCTION()
	FVector GetLoc();

private:
	UPROPERTY(EditAnywhere, Category = "PlayerMesh", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UArrowComponent* BulletSpawnPoint;

	UPROPERTY(EditAnywhere, Category = "SoundVariables")
	USoundBase* ReloadingSound;

	UPROPERTY(EditAnywhere, Category = "SoundVariables")
	USoundBase* BulletCasingSound;

	UPROPERTY(EditAnywhere, Category = "SoundVariables")
	USoundBase* DashSound;

	UPROPERTY(EditAnywhere, Category = "SoundVariables")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "SoundVariables")
	USoundBase* GunClickSound;

	UPROPERTY(EditAnywhere, Category = "SoundVariables")
	USoundBase* ErrorSound;

	UPROPERTY(EditAnywhere, Category = "SoundVariables")
	USoundBase* JumpSound;

	UPROPERTY(EditAnywhere, Category = "SoundVariables")
	USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, Category = "SoundVariables")
	USoundBase* GameOverSound;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	TSubclassOf<AActor> BulletActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UCurveFloat* DistanceCurve;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UCurveFloat* JumpCurve;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	int MaxAmmo;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float DashTimer;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float MaxSpeedBoost;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UMaterialInterface* DamageMaterial;

	UPROPERTY(VisibleAnywhere)
	TArray<UArrowComponent*> ThrustLocations;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UParticleSystemComponent* ThrustFX1;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UParticleSystemComponent* ThrustFX2;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UParticleSystemComponent* ThrustFX3;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UParticleSystemComponent* ThrustFX4;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UParticleSystem* ThrustFX;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UParticleSystem* DeathFX;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FVector InitialLocation;

	// Player input
	void Roll(float Value);
	void Pitch(float Value);

	void CameraPitch(float Value);
	void Yaw(float Value);

	void Dash();
	void ResetDash();
	void Shoot(float Value);
	void InitReload();
	void PlayBulletCasingSound();
	void Reload();
	void Jump();
	void JumpEnd();
	void ResetLocation();
	void AddHealth(float Amount);
	void EndDamageEffect();
	void PlayErrorSound();
	void EscPressed();
	void TabPressed();
	void Die();
	void GameOver();

	bool bPitchHasInput;
	bool bRollHasInput;

	float NextRollPosition;
	float NextPitchPosition;
	float NextYawPosition;

	FVector LocalMove;

	float SpeedBoost;
	float ShootTimer{};

	float CurrentYaw;
	float InitialArmLength;
	float Health;
	int Ammo;
	bool bIsReloading;
	bool bIsDashing;
	float EnemyCooldownTime;
	bool bIsJumping;
	float JumpTime;
	bool bIsStopped;
	bool bHasBeenRun1;
	bool bHasBeenRun2;
	UMaterialInterface* InitialMaterial;

	TArray<AActor*> Attackers;
	TArray<float> Timers;

	class AHUDContainer* HUDContainer;

	void Pause();
	void Unpause();
};
