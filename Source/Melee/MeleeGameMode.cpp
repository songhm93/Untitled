// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeleeGameMode.h"
#include "PlayerCharacter/BaseCharacter.h"
#include "PlayerController/MeleePlayerController.h"

AMeleeGameMode::AMeleeGameMode()
{
	// set default pawn class to our Blueprinted character
	// static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/CombatSystem/Blueprints/BP_CombatCharacter"));
	// if (PlayerPawnBPClass.Class != NULL)
	// {
	// 	DefaultPawnClass = PlayerPawnBPClass.Class;
	// }
	DefaultPawnClass = ABaseCharacter::StaticClass();
	PlayerControllerClass = AMeleePlayerController::StaticClass();

	
}

void AMeleeGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AMeleePlayerController* MPC = Cast<AMeleePlayerController>(NewPlayer);
	if(MPC)
	{
		MPC->RequestEntry();
	}

}

