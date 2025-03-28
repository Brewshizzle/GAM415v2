// Copyright Epic Games, Inc. All Rights Reserved.

#include "GAM415v2GameMode.h"
#include "GAM415v2Character.h"
#include "UObject/ConstructorHelpers.h"

AGAM415v2GameMode::AGAM415v2GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
