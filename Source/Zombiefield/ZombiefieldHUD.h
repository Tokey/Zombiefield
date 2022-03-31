// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ZombiefieldHUD.generated.h"

UCLASS()
class AZombiefieldHUD : public AHUD
{
	GENERATED_BODY()

public:
	AZombiefieldHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

