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


// Sets default values
APlayerShip::APlayerShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MyArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowSpawnPoint"));
	MyArrow->SetupAttachment(GetRootComponent());
	MyArrow->SetRelativeLocation(FVector(500.f, 0.f, -160.f));

	// Base Mesh
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> SpaceshipRef(TEXT("StaticMesh'/Game/Meshes/Spaceship/spaceship.spaceship'"));

	if (SpaceshipRef.Succeeded())
	{
		BaseMesh->SetStaticMesh(SpaceshipRef.Object);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, FString::Printf(TEXT("Spaceship mesh could not be found.")));
	}
	
	BaseMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	SetRootComponent(BaseMesh);

	// Spring Arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bDoCollisionTest = true;
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->TargetArmLength = 1500.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 10.f; // Lower = More delay
	SpringArm->SetupAttachment(GetRootComponent());

	// Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeRotation(FRotator(10.f, 0.f, 0.f));
	//float ZRot = UKismetMathLibrary::FindLookAtRotation(Camera->GetRelativeLocation(), GetActorLocation()).Pitch + 45;

	SpeedBoost = 1.f;
	DashTimer = 2.f;

	// Possess player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}


void APlayerShip::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = BaseMesh->GetComponentLocation();	
	MyArrow->SetRelativeLocation(FVector(500.f, 0.f, -160.f));
}


void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ShootTimer += DeltaTime;
	// Move ship
	AddActorLocalOffset(LocalMove, true);
	FVector Loc = GetActorLocation();
	Loc.Z = InitialLocation.Z;
	SetActorLocation(Loc);

	// Rotate ship
	SetActorRotation(FRotator(NextPitchPosition, NextYawPosition, NextRollPosition));
	FRotator SpringArmRotation = SpringArm->GetRelativeRotation();
	SpringArmRotation.Yaw = FMath::FInterpTo(SpringArmRotation.Yaw, NextYawPosition, DeltaTime, 20.f);
	SpringArmRotation.Yaw = FMath::Clamp(SpringArmRotation.Yaw, GetActorRotation().Yaw - 20.f, GetActorRotation().Yaw + 20.f);
	UE_LOG(LogTemp, Warning, TEXT("Next Yaw: %f"), NextYawPosition)
	UE_LOG(LogTemp, Warning, TEXT("Current Yaw: %f"), SpringArmRotation.Yaw)

	FRotator test = FMath::RInterpTo(SpringArm->GetComponentRotation(), GetActorRotation(), DeltaTime, 20.f);
	test.Pitch = -45.f;
	SpringArm->SetWorldRotation(test);

	


	//SpringArm->SetRelativeRotation(SpringArmRotation);



	//UE_LOG(LogTemp, Warning, TEXT("Loc: %s"), *MyArrow->GetComponentLocation().ToString())
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
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &APlayerShip::Reload);
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

		//UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Shoot", EKeys::SpaceBar));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Reload", EKeys::R));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Dash", EKeys::LeftShift));
	}
}



// -------------------------------------- CUSTOM FUNCTIONS -------------------------------------- //


void APlayerShip::ResetLocation() 
{
	FHitResult* HitResult = new FHitResult();
	SetActorLocation(FVector(0.0f), false, HitResult, ETeleportType::ResetPhysics);
}


void APlayerShip::Shoot(float Value) 
{
	if (!Value || ShootTimer < 0.10f) { return; }
	ShootTimer = 0.f;
	if (Ammo > 0) {
		Ammo--;
		GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::White, FString::Printf(TEXT("Ammo :  %d "), Ammo));

		UWorld* World = GetWorld();
		if (World)
		{
			// Bullet will spawn at the end of the barrel under the ship
			World->SpawnActor<AActor>(BulletActorToSpawn, MyArrow->GetComponentLocation(), GetActorRotation());
			UGameplayStatics::PlaySound2D(World, ShootingSound, 1.f, 1.f, 0.f, 0);
		}
		if (Ammo == 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, FString::Printf(TEXT("No ammo, reload")));
		}
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, FString::Printf(TEXT("No ammo!")));
	}
}


void APlayerShip::Reload() 
{
	Ammo = 30;
	UWorld* NewWorld = GetWorld();
	UGameplayStatics::PlaySound2D(NewWorld, ReloadingSound, 1.f, 1.f, 0.f, 0);
	GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::Green, FString::Printf(TEXT("Reloaded %d "), Ammo));
}


void APlayerShip::Pitch(float Value)
{
	// Determine if there is input
	bPitchHasInput = !(Value == 0);
	// If there is input, set rotation target to -25/25 based on input value, else set target to 0
	float TargetPitch = bPitchHasInput ? Value > 0 ? -25.0f : 25.0f : 0.f;
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
	NextYawPosition = FMath::FInterpTo(SpringArm->GetRelativeRotation().Pitch, GetActorRotation().Yaw + Value * 40.f, GetWorld()->GetDeltaSeconds(), 1.f);
}

void APlayerShip::Yaw(float Value)
{
	//NextYawPosition = GetActorRotation().Yaw + Value * 5.f;
	FRotator CurRot = GetActorRotation();
	FRotator NextRot = FRotator(0); 
	NextRot.Yaw = CurRot.Yaw + 40.f * Value;
	NextYawPosition = FMath::FInterpTo(CurRot.Yaw, NextRot.Yaw, GetWorld()->GetDeltaSeconds(), 1.f);
	//NextYawPosition = GetActorRotation().Yaw + Value;
}


void APlayerShip::Dash() 
{
	SpeedBoost = 3.f;
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerShip::ResetDash, DashTimer, false);
}

void APlayerShip::ResetDash()
{
	SpeedBoost = 1.f;
}
