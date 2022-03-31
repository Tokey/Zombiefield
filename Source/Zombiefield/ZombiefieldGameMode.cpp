// Copyright Epic Games, Inc. All Rights Reserved.

#include "ZombiefieldGameMode.h"
#include "ZombiefieldHUD.h"
#include "ZombiefieldCharacter.h"
#include "UObject/ConstructorHelpers.h"

AZombiefieldGameMode::AZombiefieldGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AZombiefieldHUD::StaticClass();
}
