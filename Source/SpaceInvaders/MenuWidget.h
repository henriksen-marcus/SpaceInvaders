// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/UMG/Public/UMG.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SPACEINVADERS_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* PlayBtn;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitBtn;

	UFUNCTION()
	void Play();

	UFUNCTION()
	void Quit();

};
