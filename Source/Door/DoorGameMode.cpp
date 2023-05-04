// Copyright Epic Games, Inc. All Rights Reserved.

#include "DoorGameMode.h"
#include "DoorCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADoorGameMode::ADoorGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
