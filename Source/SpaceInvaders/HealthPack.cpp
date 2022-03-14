// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPack.h"
#include "PlayerShip.h"
#include "Components/CapsuleComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AHealthPack::AHealthPack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthPackSize = 10.f; //Size of the HealthPack
	HealthAdded = 50.f; //Amount of Health added when pickup

	//Make the Collision component and set it as the "RootComponent"
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);

	//Get Mesh from a path and make it a child of the "RootComponent"
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);

	//Enable the OnOverlapBegin()-function to be played, when a collision on "CapsuleComponent" takes place
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AHealthPack::OnOverlapBegin);

	UE_LOG(LogTemp, Warning, TEXT("HealthPack - HealthPack Spawned"));
}

// Called when the game starts or when spawned
void AHealthPack::BeginPlay()
{
	Super::BeginPlay();

	//Scale the RootComponent and all its Childs
	RootComponent->SetWorldScale3D(FVector(HealthPackSize));
}

// Called every frame
void AHealthPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHealthPack::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APlayerShip::StaticClass()))
	{
		APlayerShip* PlayerShip = Cast<APlayerShip>(OtherActor);
		PlayerShip->AddHealth(HealthAdded);

		//Sound
		UGameplayStatics::PlaySound2D(GetWorld(), HealthPackSound);

		this->Destroy();
		UE_LOG(LogTemp, Warning, TEXT("HealthPack - Overlapping with spaceship, success!!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HealthPack - Overlapping with spaceship, but don't activate"));
	}
}