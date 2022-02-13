// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	ConstructorHelpers::FObjectFinder<UStaticMesh>BulletRef(TEXT("StaticMesh'/Game/Meshes/Bullet/BulletMesh.BulletMesh'"));

	if (BulletRef.Succeeded())
	{
		BaseMesh->SetStaticMesh(BulletRef.Object);
	}
	BaseMesh->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));
	SetRootComponent(BaseMesh);

	//RootComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	//Cast<USphereComponent>(RootComponent)->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnOverlap);
	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));

	ProjectileComp->ProjectileGravityScale = 0.5f;
	ProjectileComp->InitialSpeed = MaxBulletSpeed;

	TScriptDelegate<FWeakObjectPtr> StopDelegate;
	StopDelegate.BindUFunction(this, FName("Kill"));
	ProjectileComp->OnProjectileStop.Add(StopDelegate);

	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TriggerCapsule"));
	TriggerCapsule->InitCapsuleSize(2.f, 8.2f);
	TriggerCapsule->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	TriggerCapsule->SetRelativeLocation(FVector(2.8f, 0.f, 0.f));
	TriggerCapsule->SetCollisionProfileName(TEXT("CapsuleTrigger"));
	TriggerCapsule->SetupAttachment(GetRootComponent());
	TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnOverlapBegin);
	TriggerCapsule->OnComponentEndOverlap.AddDynamic(this, &ABullet::OnOverlapEnd);

}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

	if (BulletShootSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BulletShootSound, GetActorLocation(), 0.5f);
	}
	if (BulletFireFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletFireFX, GetActorLocation(), FRotator::ZeroRotator, FVector(0.15f));
	}
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Using projectile movement component - keeping this as backup
	/*FVector NewLocation = GetActorLocation();
	NewLocation += GetActorForwardVector() * Speed * DeltaTime;
	SetActorLocation(NewLocation);*/

	TimeLived += DeltaTime;
	if (TimeLived > TimeBeforeDestroy) { this->Destroy(); }
}


// ------------------------------ CUSTOM FUNCTIONS --------------------------- //


void ABullet::Kill()
{
	this->Destroy();
}

void ABullet::OnOverlapBegin (
	UPrimitiveComponent* OverlappedComponent, // Which self-owned component overlapped? (capsule component)
	AActor* OtherActor, // The other actor itself
	UPrimitiveComponent* OtherComponent, // The component of the other actor that has been collided with, or subobject
	int32 OtherbodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult ) 
{
	/* Do not collide with:
	 - Nothing
	 - Self
	 - Other instances of this class
	 - The PlayerShip class
	*/
	if (!OtherActor || OtherActor == this || !OtherComponent  || this->GetClass() == OtherActor->GetClass() || OtherActor->IsA(APlayerShip::StaticClass())) { return; }
	// Uncomment to see overlap information
	/*FString TACS = this->GetClass()->GetFName().ToString();
	FString OACS = OtherActor->GetClass()->GetFName().ToString();
	UE_LOG(LogTemp, Warning, TEXT("%s overlapped with %s\nThis class: %s\nOther Class: %s"), *this->GetFName().ToString(), *OtherActor->GetFName().ToString(), *TACS, *OACS);
	UE_LOG(LogTemp, Warning, TEXT("Class of other actor: %s\nClass of PlayShip %s"), *OACS, *APlayerShip::StaticClass()->GetFName().ToString())*/

	if (OtherActor->IsA(AEnemyZlorp::StaticClass()))
	{
		//APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		AEnemyZlorp* temp = Cast<AEnemyZlorp>(OtherActor);
		temp->AddHealth(-50.f); 
	}

	// Play bullet hit sound & effect then begone
	if (BulletHitSound1 && BulletHitSound2)
	{
		if (FMath::RandBool())
		{
			UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound1, 0.7f);
		}
		else {
			UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound2, 0.7f);
		}
	}
	else if (BulletHitSound1) 
	{
		UGameplayStatics::PlaySound2D(GetWorld(), BulletHitSound1, 0.7f);
	}

	if (BulletHitFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletHitFX, GetActorLocation(), FRotator::ZeroRotator, FVector(0.4f));
	}
	this->Destroy();
}


void ABullet::OnOverlapEnd (
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComponent, 
	int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT("%s stopped overlapping with %s"), *this->GetFName().ToString(), *(OtherActor)->GetFName().ToString());
}

