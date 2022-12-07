#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../Type/ItemCategory.h"
#include "InventoryInterface.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FItemInfoInSlot
{
	GENERATED_BODY()
	FItemInfoInSlot(){}
	FItemInfoInSlot(int32 _InventoryNum, int32 _ItemId, int32 _Amount, FString _ItemName, FString _Desc, UTexture2D* _Icon, FString _UseText, bool _CanUse, bool _CanStack, EItemCategory _Category, bool _IsActor, bool _Equipped, int32 _BuyGold, int32 _SellGold)
	{
		InventoryNum = _InventoryNum;
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
		Equipped = _Equipped;
		BuyGold = _BuyGold;
		SellGold = _SellGold;
	}
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 InventoryNum;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemId = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Amount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ItemName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Desc;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UTexture2D* Icon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString UseText;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanUse;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanStack;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EItemCategory Category;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool Equipped;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 BuyGold;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SellGold;
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
	virtual bool AddItem(int32 ItemId, int32 Amount, bool bFromMonster) = 0;
	virtual void AddGold(int32 GoldAmount, bool bFromMonster) = 0;
};
