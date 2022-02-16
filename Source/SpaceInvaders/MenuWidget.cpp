// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"
#include "HUDContainer.h"

void UMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayBtn->OnClicked.AddUniqueDynamic(this, &UMenuWidget::Play);
	KeyGuideBtn->OnClicked.AddUniqueDynamic(this, &UMenuWidget::ShowKeyGuide);
	QuitBtn->OnClicked.AddUniqueDynamic(this, &UMenuWidget::Quit);
}

void UMenuWidget::Play()
{
	if (this)
	{
		this->RemoveFromViewport();
	}
	

	AHUDContainer* HUDContainer = Cast<AHUDContainer>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (HUDContainer)
	{
		HUDContainer->ShowIGWidget();
	}
}


void UMenuWidget::ShowKeyGuide()
{

}


void UMenuWidget::Quit()
{

}