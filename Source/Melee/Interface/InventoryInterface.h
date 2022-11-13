#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../Type/ItemCategory.h"
#include "InventoryInterface.generated.h"

class UTexture2D;

USTRUCT()
struct FItemInfoInSlot
{
	GENERATED_BODY()
	FItemInfoInSlot(){}
	FItemInfoInSlot(int32 _ItemId, int32 _Amount, FString _ItemName, FString _Desc, UTexture2D* _Icon, FString _UseText, bool _CanUse, bool _CanStack, EItemCategory _Category, bool _IsActor)
	{
		ItemId = _ItemId;
		Amount = _Amount;
		ItemName = _ItemName;
		Desc = _Desc;
		Icon = _Icon;
		UseText = _UseText;
		CanUse = _CanUse;
		CanStack = _CanStack;
		Category = _Category;
		IsActor = _IsActor;
	}

	int32 ItemId;
	int32 Amount;
	FString ItemName;
	FString Desc;
	UTexture2D* Icon;
	FString UseText;
	bool CanUse;
	bool CanStack;
	EItemCategory Category;
	bool IsActor;
};	

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
