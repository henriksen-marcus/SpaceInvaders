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
	APlayerShip();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	static void InitializeDefaultPawnInputBinding();	


public:
	//Test
	/** Returns the location of the playership */
	UFUNCTION()
	FVector GetLoc();

	/** If true, freezes the player and prompts the HUD to display a win message */
	bool GameWon;

	void AddHealth(float Amount);

private:
	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	UBoxComponent* RtRpl; // Root replacement

	UPROPERTY(EditAnywhere, Category = "PlayerMesh")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere, Category = "CameraVariables")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UArrowComponent* BulletSpawnPoint1;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UArrowComponent* BulletSpawnPoint2;

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
	USoundBase* GameOverSound;

	UPROPERTY(EditAnywhere, Category = "SoundVariables")
	USoundBase* WinSound;

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
	UParticleSystemComponent* DeathFXComponent;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UParticleSystem* ThrustFX;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UParticleSystem* DeathFX;

	UPROPERTY()
	class AHUDContainer* HUDContainer{};
	
	UPROPERTY()
	TArray<AActor*> Attackers;

	UPROPERTY()
	UMaterialInterface* InitialMaterial;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FVector InitialLocation;

	/** Player input */
	void Roll(float Value);
	void Pitch(float Value);

	void CameraPitch(float Value);
	void Yaw(float Value);

	void Dash();
	void Shoot(float Value);
	void InitReload();
	void PlayBulletCasingSound();
	void Reload();
	void ResetDash();
	void Jump();
	void JumpEnd();
	void EndDamageEffect();
	void PlayErrorSound();
	void EscPressed();
	void TabPressed();
	void Die();
	void Win();

	bool bPitchHasInput;
	bool bRollHasInput;

	float NextRollPosition;
	float NextPitchPosition;
	float NextYawPosition;

	/** Used in AddActorLocalOffset() in Tick()*/
	FVector LocalMove;

	float SpeedBoost;
	float ShootTimer{};

	float CurrentYaw;
	float InitialArmLength;
	float Health;
	int32 Ammo;
	bool bIsReloading;
	bool bIsDashing;
	float EnemyCooldownTime;
	bool bIsJumping;
	bool IgnoreInput;
	bool temp = false;

	TArray<float> Timers;

	FVector Force = FVector::ZeroVector;
};
