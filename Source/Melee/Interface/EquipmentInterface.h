#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EquipmentInterface.generated.h"

class ABaseEquippable;

UINTERFACE(MinimalAPI)
class UEquipmentInterface : public UInterface
{
	GENERATED_BODY()
};

class MELEE_API IEquipmentInterface
{
	GENERATED_BODY()


public:
	virtual void Equip(ABaseEquippable* Weapon) = 0;
	
};
