// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDContainer.h"


void AHUDContainer::BeginPlay()
{
	Super::BeginPlay();

	if (MWidgetClass)
	{
		MWidget = CreateWidget<UMenuWidget>(GetWorld(), MWidgetClass);
		
		if (MWidget)
		{
			MWidget->AddToViewport(); // Spawn menu
			MWidget->bIsFocusable = true;
			MWidget->SetFocus();
		}
	}

	if (IGWidgetClass)
	{
		IGWidget = CreateWidget<UInGameWidget>(GetWorld(), IGWidgetClass);
	}
}


void AHUDContainer::Tick(float DeltaSeconds)
{
}


void AHUDContainer::DrawHUD()
{

}


void AHUDContainer::UpdateIGWidget(int32 Ammo, float Health)
{
	if (IGWidget)
	{
		IGWidget->Update(Ammo, Health);
	}
}


void AHUDContainer::UpdateIGWidget(int32 KillCount)
{
	if (IGWidget)
	{
		IGWidget->Update(KillCount);
	}
}


void AHUDContainer::ShowIGWidget()
{
	if (IGWidget)
	{
		IGWidget->AddToViewport();
	}
}
