// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MeleeGameMode.generated.h"


UCLASS(minimalapi)
class AMeleeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMeleeGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;


};



