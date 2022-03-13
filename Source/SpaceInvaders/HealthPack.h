// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPack.generated.h"

UCLASS()
class SPACEINVADERS_API AHealthPack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealthPack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;




	//-----------------------// MyVariables //-----------------------

public:
	UPROPERTY(EditAnywhere, Category = "Mesh")
		class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Mesh")
		class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundBase* HealthPackSound;

	UPROPERTY(EditAnywhere, Category = "Material")
		class UMaterial* MaterialComponent;

	UPROPERTY(EditAnywhere)
		class UMaterialInterface* BaseMaterial;

	UPROPERTY(EditAnywhere)
		class UMaterialInstanceDynamic* DynamicMaterial;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	float HealthPackSize;
	float HealthAdded;
};
