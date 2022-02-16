// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameWidget.h"

void UInGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
}


void UInGameWidget::Update(int32 KillCount)
{
	KillsTxt->SetText(FText::AsNumber(KillCount));
}


void UInGameWidget::Update(int32 Ammo, float Health)
{
	AmmoTxt->SetText(FText::AsNumber(Ammo));
	HealthBar->SetPercent(Health / 500);
}