// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameWidget.h"

void UInGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (KeyGuideWindow)
	{
		KeyGuideWindow->SetVisibility(ESlateVisibility::Hidden);
	}
	if (DeathScreen)
	{
		DeathScreen->SetVisibility(ESlateVisibility::Hidden);
	}
	if (WinScreen)
	{
		WinScreen->SetVisibility(ESlateVisibility::Hidden);
	}
}


void UInGameWidget::Update(int32 KillCount)
{
	if (KillsTxt)
	{
		KillsTxt->SetText(FText::AsNumber(FMath::Clamp(20 - KillCount, 0, 20)));
	}
}


void UInGameWidget::Update(int32 Ammo, float Health)
{
	if (AmmoBar && HealthBar)
	{
		AmmoBar->SetPercent(Ammo/40.f);
		HealthBar->SetPercent(Health/300);
	}
}


void UInGameWidget::ShowKeyGuide()
{
	if (KeyGuideWindow)
	{
		if (KeyGuideWindow->GetVisibility() == ESlateVisibility::Hidden)
		{
			KeyGuideWindow->SetVisibility(ESlateVisibility::Visible);
		}
		else 
		{
			KeyGuideWindow->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}


void UInGameWidget::ShowDeathScreen()
{
	if (DeathScreen)
	{
		DeathScreen->SetVisibility(ESlateVisibility::Visible);
	}
}


void UInGameWidget::ShowWinScreen()
{
	if (WinScreen)
	{
		WinScreen->SetVisibility(ESlateVisibility::Visible);
	}
}