#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryInterface.generated.h"

class AMasterItem;

UINTERFACE(MinimalAPI)
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};


class MELEE_API IInventoryInterface
{
	GENERATED_BODY()

public:
	virtual bool AddItem(int32 ItemId, int32 Amount) = 0;
	virtual void AddGold(int32 GoldAmount) = 0;
};
