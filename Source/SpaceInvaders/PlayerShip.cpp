// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerShip.h"
#include "GameFramework/PlayerInput.h"
#include "Components/InputComponent.h"
#include "bullet.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraActor.h"
#include "Engine/Engine.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/TimelineComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
APlayerShip::APlayerShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Base Mesh
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> SpaceshipRef(TEXT("StaticMesh'/Game/Meshes/Spaceship/spaceship.spaceship'"));

	if (SpaceshipRef.Succeeded())
	{
		BaseMesh->SetStaticMesh(SpaceshipRef.Object);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Spaceship mesh could not be found."))
	}

	BaseMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	SetRootComponent(BaseMesh);

	MaxAmmo = 200;
	Ammo = MaxAmmo;
	DashTimer = 2.f;
	MaxSpeedBoost = 3.f;
	SpeedBoost = 1.f;
	InitialArmLength = 1500.f;

	//Bullet spawn point
	BulletSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("BulletSpawnPoint"));
	BulletSpawnPoint->SetupAttachment(GetRootComponent());
	BulletSpawnPoint->SetRelativeLocation(FVector(500.f, 0.f, -170.f));

	// Spring Arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bDoCollisionTest = true;
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->TargetArmLength = InitialArmLength;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 10.f; // Lower = More delay
	SpringArm->SetupAttachment(GetRootComponent());

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeRotation(FRotator(10.f, 0.f, 0.f));
	//float ZRot = UKismetMathLibrary::FindLookAtRotation(Camera->GetRelativeLocation(), GetActorLocation()).Pitch + 45;

	ConstructorHelpers::FObjectFinder<UCurveFloat> CurveRef(TEXT("CurveFloat'/Game/Misc/SpringArmTargetArmLength.SpringArmTargetArmLength'"));

	if (CurveRef.Succeeded())
	{
		DistanceCurve = CurveRef.Object;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Distance Curve could not be found."))
	}

	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TriggerCapsule"));
	TriggerCapsule->InitCapsuleSize(55.f, 96.f);
	TriggerCapsule->SetCollisionProfileName(TEXT("Trigger"));
	TriggerCapsule->SetupAttachment(GetRootComponent());
	TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &APlayerShip::OnOverlapBegin);
	TriggerCapsule->OnComponentEndOverlap.AddDynamic(this, &APlayerShip::OnOverlapEnd);

	// Possess player
	//AutoPossessPlayer = EAutoReceiveInput::Player0;

	UArrowComponent* Thrust1 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust1"));
	UArrowComponent* Thrust2 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust2"));
	UArrowComponent* Thrust3 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust3"));
	UArrowComponent* Thrust4 = CreateDefaultSubobject<UArrowComponent>(TEXT("Thrust4"));

	Thrust1->SetRelativeLocationAndRotation(FVector(354.f, -432.f, -23.f), FRotator(-95.f, 0.f, 0.f));
	Thrust2->SetRelativeLocationAndRotation(FVector(354.f, 432.f, -23.f), FRotator(-95.f, 0.f, 0.f));
	Thrust3->SetRelativeLocationAndRotation(FVector(-66.f, -416.f, 82.f), FRotator(-95.f, 0.f, 0.f));
	Thrust4->SetRelativeLocationAndRotation(FVector(-66.f, 416.f, 82.f), FRotator(-95.f, 0.f, 0.f));

	ThrustLocations.Reserve(4);
	ThrustLocations.Add(Thrust1);
	ThrustLocations.Add(Thrust2);
	ThrustLocations.Add(Thrust3);
	ThrustLocations.Add(Thrust4);

	for (int i{}; i < ThrustLocations.Num(); i++) { ThrustLocations[i]->SetupAttachment(GetRootComponent()); }

	ThrustFX1 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ThrustFX1"));
	ThrustFX2 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ThrustFX2"));
	ThrustFX3 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ThrustFX3"));
	ThrustFX4 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ThrustFX4"));
	
	/**
	* The reason I am doing it like this instead of using a TArray is that UE crashes when using
	* UParticleSystemComponent->Activate() since the UParticleSystemComponent is NULL.
	* Don't know if this is a bug, but this works at least
	*/
	ThrustFX1->SetupAttachment(ThrustLocations[0]);
	ThrustFX2->SetupAttachment(ThrustLocations[1]);
	ThrustFX3->SetupAttachment(ThrustLocations[2]);
	ThrustFX4->SetupAttachment(ThrustLocations[3]);

	ThrustFX1->bAutoActivate = false;
	ThrustFX2->bAutoActivate = false;
	ThrustFX3->bAutoActivate = false;
	ThrustFX4->bAutoActivate = false;
}


void APlayerShip::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();

	/** Apply effects to all subobjects, so you don't have to assign 4 values in blueprints */
	if (ThrustFX)
	{
		ThrustFX1->SetTemplate(ThrustFX);
		ThrustFX2->SetTemplate(ThrustFX);
		ThrustFX3->SetTemplate(ThrustFX);
		ThrustFX4->SetTemplate(ThrustFX);
	}
}


void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ShootTimer += DeltaTime;

	// Move ship
	AddActorLocalOffset(LocalMove);
	FVector Loc = GetActorLocation();
	Loc.Z = InitialLocation.Z;
	SetActorLocation(Loc);

	// Rotate ship
	SetActorRotation(FRotator(NextPitchPosition, NextYawPosition, NextRollPosition));
	FRotator SpringArmRotation = SpringArm->GetRelativeRotation();
	FRotator NewRot = FMath::RInterpTo(SpringArmRotation, GetActorRotation(), DeltaTime, 25.f);
	NewRot.Pitch = SpringArmRotation.Pitch;
	NewRot.Roll = 0;
	SpringArm->SetWorldRotation(NewRot);
}


void APlayerShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent != nullptr);

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InitializeDefaultPawnInputBinding();

	PlayerInputComponent->BindAxis("Roll", this, &APlayerShip::Roll);
	PlayerInputComponent->BindAxis("Pitch", this, &APlayerShip::Pitch);

	PlayerInputComponent->BindAxis("CameraPitch", this, &APlayerShip::CameraPitch);
	PlayerInputComponent->BindAxis("Yaw", this, &APlayerShip::Yaw);
	
	PlayerInputComponent->BindAxis("Shoot", this, &APlayerShip::Shoot);

	PlayerInputComponent->BindAction("Dash", EInputEvent::IE_Pressed, this, &APlayerShip::Dash);
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &APlayerShip::InitReload);
}


// Hardcode input mappings
void APlayerShip::InitializeDefaultPawnInputBinding()
{
	static bool bindingsAdded = false;

	if (bindingsAdded == false)
	{
		bindingsAdded = true;

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Pitch", EKeys::W, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Pitch", EKeys::S, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Roll", EKeys::A, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Roll", EKeys::D, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CameraPitch", EKeys::MouseY, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Yaw", EKeys::MouseX, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Shoot", EKeys::SpaceBar, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Shoot", EKeys::LeftMouseButton, 1.f));

		//UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Shoot", EKeys::SpaceBar));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Reload", EKeys::R));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Dash", EKeys::LeftShift));
	}
}



// -------------------------------------- CUSTOM FUNCTIONS -------------------------------------- //

void APlayerShip::Pitch(float Value)
{
	// Determine if there is input
	bPitchHasInput = !(Value == 0);
	// If there is input, set rotation target to -25/25 based on input value, else set target to 0
	float TargetPitch = bPitchHasInput ? Value > 0.f ? -25.0f : 25.0f : 0.f;

	// Interpolate rotation towards target
	NextPitchPosition = FMath::FInterpTo(GetActorRotation().Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 6.0f);
	float TargetXSpeed = bPitchHasInput ? (Value * 30.f * SpeedBoost) : 0.f;
	LocalMove.X = FMath::FInterpTo(LocalMove.X, TargetXSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}


void APlayerShip::Roll(float Value)
{
	// Determine if there is input
	bRollHasInput = !(Value == 0);
	// If there is input, set rotation target to -30/30 based on input value, else set target to 0
	float TargetRoll = bRollHasInput ? Value > 0 ? -30.0f : 30.0f : 0.f;
	// Interpolate rotation towards target
	NextRollPosition = FMath::FInterpTo(GetActorRotation().Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 7.5f);

	float TargetYSpeed = bRollHasInput ? (Value * -30.f) : 0.f;
	LocalMove.Y = FMath::FInterpTo(LocalMove.Y, TargetYSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}


void APlayerShip::CameraPitch(float Value)
{
	if (!Value) { return; }
	FRotator CurrentRot = SpringArm->GetRelativeRotation();
	float TargetPitch = FMath::Clamp(CurrentRot.Pitch + Value * 15.f, -70.f, 0.f);
	CurrentRot.Pitch = FMath::FInterpTo(CurrentRot.Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 10.f);

	SpringArm->SetRelativeRotation(CurrentRot);
	SpringArm->TargetArmLength = InitialArmLength * DistanceCurve->GetFloatValue(CurrentRot.Pitch);
}

void APlayerShip::Yaw(float Value)
{
	NextYawPosition = GetActorRotation().Yaw + Value * GetWorld()->GetDeltaSeconds() * 50.f;
}


void APlayerShip::Dash() 
{
	SpeedBoost = MaxSpeedBoost;

	if (DashSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), DashSound, GetActorLocation());
	}
	if (ThrustFX)
	{
		// Old, keeping in case I need it
		/*UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ThrustFX, Loc, Rot, FVector(0.4f));
		UGameplayStatics::SpawnEmitterAttached(ThrustFX, ThrustLocations[i], FName(EName::NAME_None), Loc, Rot, FVector(0.5f));*/
		ThrustFX1->ResetParticles();
		ThrustFX2->ResetParticles();
		ThrustFX3->ResetParticles();
		ThrustFX4->ResetParticles();

		ThrustFX1->Activate();
		ThrustFX2->Activate();
		ThrustFX3->Activate();
		ThrustFX4->Activate();
	}

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerShip::ResetDash, DashTimer, false);
}


void APlayerShip::ResetDash()
{
	SpeedBoost = 1.f;
}


void APlayerShip::ResetLocation()
{
	SetActorLocation(InitialLocation);
}


void APlayerShip::Shoot(float Value)
{
	if (!Value || ShootTimer < 0.08f) { return; }
	ShootTimer = 0.f;
	if (Ammo > 0) {
		Ammo--;
		UWorld* World = GetWorld();
		if (World)
		{
			FRotator Rot = GetActorRotation();
			Rot.Pitch = 0;
			// Bullet will spawn at the end of the barrel under the ship
			World->SpawnActor<AActor>(BulletActorToSpawn, BulletSpawnPoint->GetComponentLocation(), Rot);
			LocalMove.X -= 0.8f;
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Red, FString::Printf(TEXT("No ammo, reload!")));
	}
}


/** We need an init function to start the timer for the main function */
void APlayerShip::InitReload()
{
	FTimerHandle handle, handle2;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerShip::Reload, 1.f, false);
	GetWorld()->GetTimerManager().SetTimer(handle2, this, &APlayerShip::PlayBulletCasingSound, 0.57f, false);

	UGameplayStatics::PlaySound2D(GetWorld(), ReloadingSound, 2.f);
}


void APlayerShip::PlayBulletCasingSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), BulletCasingSound, 0.4f);
}


void APlayerShip::Reload()
{
	GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Green, FString::Printf(TEXT("Reloaded!")));
	Ammo = 200;
}


void APlayerShip::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if (!OtherActor || OtherActor == this || !OtherComponent || OtherActor->IsA(ABullet::StaticClass())) { return; }
	UE_LOG(LogTemp, Warning, TEXT("Taking damage!"))
}


void APlayerShip::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
}


FVector APlayerShip::GetLoc()
{
	return GetActorLocation();
}
