// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));

	ConstructorHelpers::FObjectFinder<UStaticMesh>BulletRef(TEXT("StaticMesh'/Game/Meshes/Bullet/BulletMesh.BulletMesh'"));

	BaseMesh->SetStaticMesh(BulletRef.Object);
	BaseMesh->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));
	SetRootComponent(BaseMesh);
	//RootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	//Cast<USphereComponent>(RootComponent)->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnOverlap);
	
	ProjComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("HelloBullet"));
	BaseMesh->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnOverlap);
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//FVector NewLocation = GetActorLocation();
	//NewLocation += GetActorForwardVector() * Speed * DeltaTime;
	//SetActorLocation(NewLocation);
	TimeLived += DeltaTime;

	if (TimeLived > TimeBeforeDestroy)
	{
		this->Destroy();
	}
}


// ------------------------------ CUSTOM FUNCTIONS --------------------------- //


void ABullet::OnOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComponent, 
	int32 OtherbodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult) 
{

	UE_LOG(LogTemp, Warning, TEXT("Overlapped with %s"), *(OtherActor)->GetFName().ToString());
	this->Destroy();
}

