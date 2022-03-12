// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "InGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class SPACEINVADERS_API UInGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Update(int32 KillCount);
	void Update(int32 Ammo, float Health);

	void ShowKeyGuide();
	void ShowDeathScreen();
	void ShowWinScreen();

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar{};

	UPROPERTY(meta = (BindWidget))
	UProgressBar* AmmoBar{};

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillsTxt{};

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* KeyGuideWindow{};

	UPROPERTY(meta = (BindWidget))
	UImage* DeathScreen{};

	UPROPERTY(meta = (BindWidget))
	UImage* WinScreen{};
};
