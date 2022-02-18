// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "PlayerShip.h"
#include "EnemyZlorp.h"
#include "Bullet.generated.h"

UCLASS()
class SPACEINVADERS_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	ABullet();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Test", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileComp;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	bool KillBounce = false;

private:
	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TimeBeforeDestroy;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float TimeLived;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	float MaxBulletSpeed;
	
	UPROPERTY(EditAnywhere, Category = "CapsuleCollision")
	UCapsuleComponent* TriggerCapsule;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	USoundBase* BulletShootSound;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	USoundBase* BulletHitSound1;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	USoundBase* BulletHitSound2;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UParticleSystem* BulletFireFX;

	UPROPERTY(EditAnywhere, Category = "EditableVariables")
	UParticleSystem* BulletHitFX;

	UFUNCTION()
	void Kill();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
