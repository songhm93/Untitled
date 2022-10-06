#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};


class MELEE_API ICombatInterface
{
	GENERATED_BODY()

public:
	virtual void ContinueAttack() = 0;
	virtual void ResetAttack() = 0;
	virtual FRotator GetDesiredRotation() = 0;
	virtual void ResetCombat() = 0;
};
