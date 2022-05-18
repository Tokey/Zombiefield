// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIEFIELD_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void UpdateAmmoText(FString Text);

	UFUNCTION(BlueprintCallable)
		void UpdateScoreText(FString Text);

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AmmoText;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ScoreText;

	void NativeConstruct() override;
};
