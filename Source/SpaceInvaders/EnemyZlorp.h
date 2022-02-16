// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyZlorp.generated.h"

UCLASS()
class SPACEINVADERS_API AEnemyZlorp : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyZlorp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	FVector TargetVector;

	/** Returns the health of the Zlorp */
	UFUNCTION()
	float GetHealth();

	/** Plusses ChangeAmount to the Zlorp's health, can be negative for damage */
	UFUNCTION()
	void AddHealth(float ChangeAmount);

	UPROPERTY(EditAnywhere, Category = "ZlorpVariables")
	float Health;

private:
	UPROPERTY(EditAnywhere, Category = "ZlorpVariables")
	float ZlorpSpeed;

	UPROPERTY(EditAnywhere, Category = "ZlorpVariables")
	UParticleSystem* DeathFX;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UMaterialInterface* DamageMaterial;

	UMaterialInterface* InitialMaterial0;
	UMaterialInterface* InitialMaterial1;

	float PrevSpeed;
	float TargetSpeed;
	float TargetOffset;
	float Timer;

	void EndDamageEffect();
};
