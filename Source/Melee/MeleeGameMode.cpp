// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeleeGameMode.h"
#include "MeleeCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMeleeGameMode::AMeleeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
