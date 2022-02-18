// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"
#include "SpaceInvadersGameModeBase.h"

void UMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/** Assign functions to each button */
	PlayBtn->OnClicked.AddUniqueDynamic(this, &UMenuWidget::Play);
	NoEnemiesBtn->OnClicked.AddUniqueDynamic(this, &UMenuWidget::NoEnemies);
	QuitBtn->OnClicked.AddUniqueDynamic(this, &UMenuWidget::Quit);
}

void UMenuWidget::Play()
{
	/** Hide menu from player's viewport */
	if (GetWorld())
	{
		GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
		this->RemoveFromViewport();
	}
	
	/** Show the in-game HUD and start the game */
	AHUDContainer* HUDContainer = Cast<AHUDContainer>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (HUDContainer)
	{
		HUDContainer->ShowIGWidget();
		ASpaceInvadersGameModeBase* GameMode = Cast<ASpaceInvadersGameModeBase>(GetWorld()->GetAuthGameMode());

		if (GameMode)
		{
			GameMode->bGameStarted = true;
		}
	}
}


void UMenuWidget::NoEnemies()
{
	/** Just remove from viewport without allowing GameModeBase to spawn enemies */
	if (GetWorld())
	{
		GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
		this->RemoveFromViewport();
	}

	/** Show the in-game HUD */
	AHUDContainer* HUDContainer = Cast<AHUDContainer>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (HUDContainer)
	{
		HUDContainer->ShowIGWidget();
	}
}


void UMenuWidget::Quit()
{
	FGenericPlatformMisc::RequestExit(false);
}