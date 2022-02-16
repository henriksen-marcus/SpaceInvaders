// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"
#include "SpaceInvadersGameModeBase.h"

void UMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayBtn->OnClicked.AddUniqueDynamic(this, &UMenuWidget::Play);
	QuitBtn->OnClicked.AddUniqueDynamic(this, &UMenuWidget::Quit);
}

void UMenuWidget::Play()
{
	if (this)
	{
		GetWorld()->GetFirstPlayerController()->SetInputMode(FInputModeGameOnly());
		GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
		this->RemoveFromViewport();
	}
	
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

void UMenuWidget::Quit()
{
	FGenericPlatformMisc::RequestExit(false);
}