// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeleeGameMode.h"
#include "Player/MeleeCharacter.h"
#include "PlayerController/MeleePlayerController.h"

AMeleeGameMode::AMeleeGameMode()
{
	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/CombatSystem/Blueprints/BP_CombatCharacter"));
	// if (PlayerPawnBPClass.Class != NULL)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }
	DefaultPawnClass = AMeleeCharacter::StaticClass();
	PlayerControllerClass = AMeleePlayerController::StaticClass();
}
