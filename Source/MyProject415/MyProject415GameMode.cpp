// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProject415GameMode.h"
#include "MyProject415Character.h"
#include "UObject/ConstructorHelpers.h"

AMyProject415GameMode::AMyProject415GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
