#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../Type/Types.h"
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
	virtual void ResetCombat() = 0; 
	virtual bool CanRecieveDamage() = 0;
	virtual float PerformCombatAction(ECurrentAction Action, ECurrentState State) = 0; 

};
