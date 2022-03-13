// Fill out your copyright notice in the Description page of Project Settings.


#include "HelathPack.h"
#include "PlayerShip.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AHelathPack::AHelathPack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthPackSize = 1.f; //Size of the HealthPack

}

// Called when the game starts or when spawned
void AHelathPack::BeginPlay()
{
	Super::BeginPlay();
	
	//Make the Collision component and set it as the "RootComponent"
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);

	//Get Mesh from a path and make it a child of the "RootComponent"
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponentAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule'"));
	if (MeshComponentAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(MeshComponentAsset.Object);
	}
}

// Called every frame
void AHelathPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHelathPack::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APlayerShip::StaticClass()))
	{
		APlayerShip* PlayerShip = Cast<APlayerShip>(OtherActor);
		PlayerShip->AddHealth(50.f);
	}
}