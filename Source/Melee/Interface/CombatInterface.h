#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../Type/DamageTypes.h"
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
	virtual bool CanReceiveDamage() = 0;
	virtual void CalcReceiveDamage(float ATK) = 0;
	virtual void ApplyHitReaction(EDamageType DamageType) = 0;
	virtual void ApplyImpactEffect() = 0;
	virtual bool CalcCritical(float Percent) = 0;
};
