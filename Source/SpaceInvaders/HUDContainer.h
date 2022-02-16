// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Components/WidgetComponent.h"
#include "InGameWidget.h"
#include "MenuWidget.h"
#include "HUDContainer.generated.h"

/**
 * 
 */
UCLASS()
class SPACEINVADERS_API AHUDContainer : public AHUD
{
	GENERATED_BODY()
	
public:
	//AHUDContainer();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void DrawHUD() override;


	void UpdateIGWidget(int32 Ammo, float Health);
	void UpdateIGWidget(int32 KillCount);

	void ShowIGWidget();

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> MWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> IGWidgetClass;
	
	UMenuWidget* MWidget;
	UInGameWidget* IGWidget;
};
