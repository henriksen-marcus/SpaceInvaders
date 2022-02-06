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


// Sets default values
APlayerShip::APlayerShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerMesh"));
	SetRootComponent(BaseMesh);
	ConstructorHelpers::FObjectFinder<UStaticMesh> SpaceshipRef(TEXT("StaticMesh'/Game/Meshes/Spaceship/spaceship.spaceship'"));
	BaseMesh->SetStaticMesh(SpaceshipRef.Object);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->bDoCollisionTest = true;
	//SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	SpringArm->TargetArmLength = 2000.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 15.f;
	SpringArm->SetupAttachment(BaseMesh);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	//float ZRot = UKismetMathLibrary::FindLookAtRotation(Camera->GetRelativeLocation(), GetActorLocation()).Pitch + 45;
	Camera->SetRelativeRotation(FRotator(10.f, 0.f, 0.f));

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}


void APlayerShip::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = BaseMesh->GetComponentLocation();
}


void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	InContact = false;
	//BaseMesh->SetRelativeLocation(FVector(XValue, YValue, 0.f));
	AddActorLocalOffset(OffsetVector);
	//SpringArm->SetRelativeRotation(FRotator(-10.f, GetActorRotation().Yaw, GetActorRotation().Roll));
	UE_LOG(LogTemp, Warning, TEXT("Speed: %s"), *OffsetVector.ToString());
}


void APlayerShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent != nullptr);

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InitializeDefaultPawnInputBinding();
	PlayerInputComponent->BindAxis("Forward", this, &APlayerShip::Forward);
	PlayerInputComponent->BindAxis("Right", this, &APlayerShip::Right);

	PlayerInputComponent->BindAxis("Pitch", this, &APlayerShip::Pitch);
	PlayerInputComponent->BindAxis("Yaw", this, &APlayerShip::Yaw);

	PlayerInputComponent->BindAction("Dash", EInputEvent::IE_Pressed, this, &APlayerShip::Dash);
	PlayerInputComponent->BindAction("Shoot", EInputEvent::IE_Pressed, this, &APlayerShip::Shoot);
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &APlayerShip::Reload);
}


// Hardcode input mappings
void APlayerShip::InitializeDefaultPawnInputBinding()
{
	static bool bindingsAdded = false;

	if (bindingsAdded == false)
	{
		bindingsAdded = true;

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Forward", EKeys::W, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Forward", EKeys::S, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Right", EKeys::D, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Right", EKeys::A, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Pitch", EKeys::NumPadFive, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Pitch", EKeys::NumPadEight, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Yaw", EKeys::NumPadFour, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Yaw", EKeys::NumPadSix, -1.f));

		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Shoot", EKeys::SpaceBar));
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


void APlayerShip::Shoot() 
{
	if (Ammo > 0) {
		Ammo--;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Ammo : !"), FString::FromInt(Ammo)); 
		GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::White, FString::Printf(TEXT("Ammo :  %d "), Ammo));


		UWorld* World = GetWorld();
		if (World)
		{
			FVector Location = GetActorLocation();
			//150cm ahead of actor the bullet will be spawn 
			World->SpawnActor<AActor>(BulletActorToSpawn, Location + FVector(150.f, 0.f, 0.f), GetActorRotation());
			UGameplayStatics::PlaySound2D(World, ShootingSound, 1.f, 1.f, 0.f, 0);
		}
		if (Ammo == 0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::Red, FString::Printf(TEXT("No ammo Reload %d "), Ammo));
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Shooting"));
}


void APlayerShip::Reload() 
{
	Ammo = 30;
	UWorld* NewWorld = GetWorld();
	UGameplayStatics::PlaySound2D(NewWorld, ReloadingSound, 1.f, 1.f, 0.f, 0);
	GEngine->AddOnScreenDebugMessage(-1, 12.f, FColor::Green, FString::Printf(TEXT("Reloaded %d "), Ammo));
}


void APlayerShip::Forward(float Value) 
{
	if (!Value && OffsetVector.X > 0.04) 
	{
		//Air resistance
		OffsetVector.X -= 0.04;
	}
	else if (Value > 0) {
		OffsetVector.X += Value * SpeedMultiplier / 5;
	}
	else if (Value < 0) {
		OffsetVector.X += Value * SpeedMultiplier / 10;
	}
}


void APlayerShip::Right(float Value) 
{
	FRotator NewRot = GetActorRotation();
	NewRot.Yaw += Value;
	SetActorRelativeRotation(NewRot);
}


void APlayerShip::Pitch(float Value)
{

}


void APlayerShip::Yaw(float Value)
{

}


void APlayerShip::Dash() 
{
	FVector CurrentLoc = GetActorLocation();
	FVector NewLoc = CurrentLoc;
	NewLoc.X += 2000;
	FMath::VInterpTo(CurrentLoc, NewLoc, 0.001, 2);
}
