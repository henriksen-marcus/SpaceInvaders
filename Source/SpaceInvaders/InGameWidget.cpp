// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameWidget.h"

void UInGameWidget::NativeConstruct()
{
	Super::NativeConstruct();
}


void UInGameWidget::Update(int32 KillCount, int32 AmmoCount, float HealthAmount)
{
	KillsTxt->SetText(FText::AsNumber(KillCount));
	AmmoTxt->SetText(FText::AsNumber(AmmoCount));
	HealthBar->SetPercent(HealthAmount / 500);
}