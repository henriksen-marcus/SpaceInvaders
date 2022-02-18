// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerShip.h"
#include "GameFramework/PlayerInput.h"
#include "Components/InputComponent.h"
#include "Bullet.h"
#include "Engine/World.h"
#include "Camera/CameraActor.h"
#include "Engine/Engine.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "GenericPlatform/GenericPlatformMisc.h"

// Sets default values
APlayerShip::APlayerShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/** Spaceship mesh */
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

	/** I can actually just use the mesh as a collision detector, as I have assigned 
	simplified 26DOP collision to it in the mesh editor */
	BaseMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BaseMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	BaseMesh->OnComponentBeginOverlap.AddDynamic(this, &APlayerShip::OnOverlapBegin);

	MaxAmmo = 40;
	Ammo = MaxAmmo;
	DashTimer = 2.f;
	MaxSpeedBoost = 4.f;
	SpeedBoost = 1.f;
	InitialArmLength = 1000.f;
	MaxHealth = 300.f;
	Health = MaxHealth;
	InitialMaterial = BaseMesh->GetMaterial(0);
	bIsReloading = false;
	bIsDashing = false;
	bIsJumping = false;
	JumpTime = 0.f;
	EnemyCooldownTime = 1.f;
	IgnoreInput = false;
	GameWon = false;

	BulletSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("BulletSpawnPoint"));
	BulletSpawnPoint->SetupAttachment(GetRootComponent());
	BulletSpawnPoint->SetRelativeLocation(FVector(500.f, 0.f, -170.f));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->TargetArmLength = InitialArmLength;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 25.f; // Lower = More delay
	SpringArm->bDoCollisionTest = false;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeRotation(FRotator(10.f, 0.f, 0.f));

	/** Curve for camera distance when pitching the camera */
	ConstructorHelpers::FObjectFinder<UCurveFloat> CurveRef(TEXT("CurveFloat'/Game/Misc/SpringArmTargetArmLength.SpringArmTargetArmLength'"));

	if (CurveRef.Succeeded())
	{
		DistanceCurve = CurveRef.Object;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Distance Curve could not be found."))
	}

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

	DeathFXComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DeathFX"));
	DeathFXComponent->SetupAttachment(GetRootComponent());
	DeathFXComponent->bAutoActivate = false;
	DeathFXComponent->SetRelativeScale3D(FVector(7.f, 7.f, 7.f));
}


void APlayerShip::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();

	/** Apply effects to all subobjects, so you don't have to assign 4 individual values in blueprints */
	if (ThrustFX)
	{
		ThrustFX1->SetTemplate(ThrustFX);
		ThrustFX2->SetTemplate(ThrustFX);
		ThrustFX3->SetTemplate(ThrustFX);
		ThrustFX4->SetTemplate(ThrustFX);
	}

	if (DeathFX)
	{
		DeathFXComponent->SetTemplate(DeathFX);
	}
	
	HUDContainer = Cast<AHUDContainer>(GetWorld()->GetFirstPlayerController()->GetHUD());

	if (HUDContainer)
	{
		HUDContainer->UpdateIGWidget(Ammo, Health);
	}
}


void APlayerShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ShootTimer += DeltaTime;

	/** Ship movement */
	AddActorLocalOffset(LocalMove * DeltaTime * 110.f, true);
	/** Constant Z location and jumping functionality */
	FVector Loc = GetActorLocation();
	if (bIsJumping && JumpCurve)
	{
		Loc.Z = InitialLocation.Z + JumpCurve->GetFloatValue(JumpTime) * 1500;
		JumpTime += DeltaTime;
	}
	else {
		Loc.Z = InitialLocation.Z;
	}
	SetActorLocation(Loc, true);

	SetActorRotation(FRotator(NextPitchPosition, NextYawPosition, NextRollPosition));

	/** Springarm rotation */
	FRotator SpringArmRotation = SpringArm->GetRelativeRotation();
	FRotator NewRot = FMath::RInterpTo(SpringArmRotation, GetActorRotation(), DeltaTime, 25.f);
	NewRot.Pitch = SpringArmRotation.Pitch;
	NewRot.Roll = 0;
	SpringArm->SetWorldRotation(NewRot);

	/** Game has ended if one of these are true */
	if (IgnoreInput || GameWon)
	{
		IgnoreInput = true;

		if (!Health)
		{
			/** Run only once */
			static bool HasDied;
			if (!HasDied)
			{
				Die();
				HasDied = true;
			}
		}
		else 
		{
			/** Run only once */
			static bool HasWon;
			if (!HasWon)
			{
				Win();
				HasWon = true;
			}
		}
		
		/** Since input is blocked, interp movement to zero */
		LocalMove = FMath::VInterpTo(LocalMove, FVector::ZeroVector, DeltaTime, 3.f);
		NextPitchPosition = FMath::FInterpTo(NextPitchPosition, 0.f, DeltaTime, 1.f);
		NextRollPosition = FMath::FInterpTo(NextRollPosition, 0.f, DeltaTime, 1.f);
	}
	
	/** Timer system for enemies, see function OnOverlapBegin() */
	for (int i{}; i < Timers.Num(); i++)
	{
		Timers[i] += DeltaTime;
		if (Timers[i] >= 1.5f)
		{
			Timers.RemoveAt(i);
			Attackers.RemoveAt(i);
		}
	}

	/** Update in-game HUD */
	if (HUDContainer)
	{
		HUDContainer->UpdateIGWidget(Ammo, Health);
	}
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

	/** Shoot is an axis binding so that you can hold to shoot fast */
	PlayerInputComponent->BindAxis("Shoot", this, &APlayerShip::Shoot);

	PlayerInputComponent->BindAction("Dash", EInputEvent::IE_Pressed, this, &APlayerShip::Dash);
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &APlayerShip::InitReload);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &APlayerShip::Jump);
	PlayerInputComponent->BindAction("Esc", EInputEvent::IE_Pressed, this, &APlayerShip::EscPressed);
	PlayerInputComponent->BindAction("Tab", EInputEvent::IE_Pressed, this, &APlayerShip::TabPressed);
	PlayerInputComponent->BindAction("Tab", EInputEvent::IE_Released, this, &APlayerShip::TabPressed);
}


/** Hardcode input mappings */
void APlayerShip::InitializeDefaultPawnInputBinding()
{
	static bool BindingsAdded = false;

	if (BindingsAdded == false)
	{
		BindingsAdded = true;

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Pitch", EKeys::W, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Pitch", EKeys::S, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Roll", EKeys::A, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Roll", EKeys::D, -1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("CameraPitch", EKeys::MouseY, 1.f));
		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Yaw", EKeys::MouseX, 1.f));

		UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Shoot", EKeys::LeftMouseButton, 1.f));

		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Jump", EKeys::SpaceBar));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Reload", EKeys::R));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Dash", EKeys::LeftShift));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Esc", EKeys::Escape));
		UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Tab", EKeys::Tab));
	}
}


// -------------------------------------- CUSTOM FUNCTIONS -------------------------------------- //

void APlayerShip::Pitch(float Value)
{
	if (IgnoreInput) { return; }

	// Determine if there is input
	bPitchHasInput = !(Value == 0);
	// If there is input, set rotation target to -25/25 based on input value, else set target to 0
	float TargetPitch = bPitchHasInput ? Value > 0.f ? -25.0f : 25.0f : 0.f;

	// Interpolate rotation towards target
	NextPitchPosition = FMath::FInterpTo(GetActorRotation().Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 6.0f);
	float TargetXSpeed = bPitchHasInput ? (Value * 40.f * SpeedBoost) : 0.f;
	LocalMove.X = FMath::FInterpTo(LocalMove.X, TargetXSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}


void APlayerShip::Roll(float Value)
{
	if (IgnoreInput) { return; }

	// Determine if there is input
	bRollHasInput = !(Value == 0);
	// If there is input, set rotation target to -30/30 based on input value, else set target to 0
	float TargetRoll = bRollHasInput ? Value > 0 ? -30.0f : 30.0f : 0.f;
	// Interpolate rotation towards target
	NextRollPosition = FMath::FInterpTo(GetActorRotation().Roll, TargetRoll, GetWorld()->GetDeltaSeconds(), 7.5f);

	float TargetYSpeed = bRollHasInput ? (Value * -30.f) : 0.f;
	LocalMove.Y = FMath::FInterpTo(LocalMove.Y, TargetYSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}


void APlayerShip::Yaw(float Value)
{
	if (IgnoreInput) { return; }
	NextYawPosition = GetActorRotation().Yaw + Value * GetWorld()->GetDeltaSeconds() * 50.f;
}


void APlayerShip::CameraPitch(float Value)
{
	if (!Value || IgnoreInput) { return; }
	FRotator CurrentRot = SpringArm->GetRelativeRotation();
	float TargetPitch = FMath::Clamp(CurrentRot.Pitch + Value * 15.f, -50.f, 0.f);
	CurrentRot.Pitch = FMath::FInterpTo(CurrentRot.Pitch, TargetPitch, GetWorld()->GetDeltaSeconds(), 10.f);

	SpringArm->SetRelativeRotation(CurrentRot);
	SpringArm->TargetArmLength = InitialArmLength * DistanceCurve->GetFloatValue(CurrentRot.Pitch);
}


void APlayerShip::Dash() 
{
	if (IgnoreInput) { return; }

	if (bIsDashing) 
	{
		PlayErrorSound();
		return;
	}

	SpeedBoost = MaxSpeedBoost;
	bIsDashing = true;

	if (DashSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), DashSound);
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
	bIsDashing = false;
}


/** Can be used for health pack functionality or to take damage */
void APlayerShip::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);

	if (Amount < 0 && DamageMaterial && HitSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), HitSound, 0.7f);
		BaseMesh->SetMaterial(0, DamageMaterial);

		FTimerHandle handle;
		GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerShip::EndDamageEffect, 0.1f, false);
	}

	if (!Health)
	{
		IgnoreInput = true;
	}
}


/** Reset material */
void APlayerShip::EndDamageEffect()
{
	BaseMesh->SetMaterial(0, InitialMaterial);
}


void APlayerShip::PlayErrorSound()
{
	if (ErrorSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), ErrorSound, 0.8f);
	}
}


void APlayerShip::Shoot(float Value)
{
	if (!Value || ShootTimer < 0.085f || IgnoreInput) { return; }

	ShootTimer = 0.f;

	if (Ammo > 0) {
		--Ammo;
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
	else if (GunClickSound) 
	{
		UGameplayStatics::PlaySound2D(GetWorld(), GunClickSound, 0.8f);
	}
}


/** We need an init function to start the timer for the main function */
void APlayerShip::InitReload()
{
	if (IgnoreInput) { return; }

	if (bIsReloading)
	{
		PlayErrorSound();
		return;
	}

	bIsReloading = true;
	UGameplayStatics::PlaySound2D(GetWorld(), ReloadingSound, 4.f);
	FTimerHandle handle, handle2;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerShip::Reload, 1.f, false);
	GetWorld()->GetTimerManager().SetTimer(handle2, this, &APlayerShip::PlayBulletCasingSound, 0.57f, false);
}


void APlayerShip::PlayBulletCasingSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), BulletCasingSound, 0.4f);
}


void APlayerShip::Reload()
{
	Ammo = MaxAmmo;
	bIsReloading = false;
}


void APlayerShip::Jump()
{
	if (IgnoreInput) { return; }

	if (bIsJumping)
	{
		PlayErrorSound();
		return;
	}

	bIsJumping = true;
	UGameplayStatics::PlaySound2D(GetWorld(), JumpSound, 0.5f);
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, this, &APlayerShip::JumpEnd, 2.f, false);

	if (ThrustFX)
	{
		ThrustFX1->ResetParticles();
		ThrustFX2->ResetParticles();
		ThrustFX3->ResetParticles();
		ThrustFX4->ResetParticles();

		ThrustFX1->Activate();
		ThrustFX2->Activate();
		ThrustFX3->Activate();
		ThrustFX4->Activate();
	}
}


void APlayerShip::JumpEnd()
{
	bIsJumping = false;
	JumpTime = 0.f;
}


void APlayerShip::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherbodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || !OtherComponent || OtherActor->IsA(ABullet::StaticClass()) || IgnoreInput) { return; }

	/** Check if the overlapping object is an enemy */
	if (OtherActor->IsA(AEnemyZlorp::StaticClass()))
	{
		/** Cooldown system; Makes sure you don't get instakilled by overlapping as it relies on tick.  */
		bool bIsOnCooldown = false;
		for (int i{}; i < Attackers.Num(); i++)
		{
			if (Attackers[i] == OtherActor) {
				bIsOnCooldown = true;
				break;
			}
		}

		if (!bIsOnCooldown)
		{
			AddHealth(-50.f);
			Attackers.Add(OtherActor);
			Timers.Add(0.f);
		}
	}
}


FVector APlayerShip::GetLoc()
{
	return GetActorLocation();
}


void APlayerShip::EscPressed()
{
	//UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
	FGenericPlatformMisc::RequestExit(false);
}


void APlayerShip::TabPressed()
{
	if (HUDContainer)
	{
		HUDContainer->IGWidget->ShowKeyGuide();
	}
}


void APlayerShip::Die()
{
	if (DeathFXComponent)
	{
		DeathFXComponent->ResetParticles();
		DeathFXComponent->Activate();
	}
	if (GameOverSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), GameOverSound, 2.3f);
	}
	if (HUDContainer)
	{
		HUDContainer->IGWidget->ShowDeathScreen();
	}
}


void APlayerShip::Win()
{
	if (WinSound)
	{
		/** Game over sound just with higer pitch */
		UGameplayStatics::PlaySound2D(GetWorld(), WinSound, 1.f);
	}
	if (HUDContainer)
	{
		HUDContainer->IGWidget->ShowWinScreen();
	}
}