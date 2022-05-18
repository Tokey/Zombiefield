// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoWidget.h"
#include <Components/TextBlock.h>



void UAmmoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateAmmoText("DUMMY TEXT");
	UpdateScoreText("DUMMY Score");
}

void UAmmoWidget::UpdateAmmoText(FString Text)
{
	AmmoText->SetText(FText::FromString(Text));
}

void UAmmoWidget::UpdateScoreText(FString Text)
{
	ScoreText->SetText(FText::FromString("Score: " + Text));
}
