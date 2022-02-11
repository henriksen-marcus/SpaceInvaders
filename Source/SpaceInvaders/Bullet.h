// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Bullet.generated.h"

UCLASS()
class SPACEINVADERS_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


private:

	UPROPERTY(EditAnywhere, Category = "CustomVars")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere, Category = "CustomVars")
	float Speed = 5000.f;

	UPROPERTY(EditAnywhere, Category = "CustomVars")
	float TimeBeforeDestroy = 20.f;

	UPROPERTY(EditAnywhere, Category = "CustomVars")
	float TimeLived = 0.f;

	UPROPERTY(EditAnywhere, Category = "CustomVars")
	UParticleSystem* BulletFireFX = nullptr;

	UPROPERTY(EditAnywhere, Category = "CustomVars")
	USoundBase* BulletFireSound = nullptr;

	UPROPERTY(EditAnywhere)
	class UProjectileMovementComponent* ProjComp;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
