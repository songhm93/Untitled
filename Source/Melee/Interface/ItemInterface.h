#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/DataTable.h"
#include "../Type/ItemCategory.h"
#include "../Type/Types.h"
#include "ItemInterface.generated.h"


class UTexture2D;
class ABaseWeapon;

USTRUCT(BlueprintType)
struct FItemInfoTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Itemid;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Desc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Usetext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Canuse;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Canstack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemCategory Category;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Isactor;
};

USTRUCT(BlueprintType)
struct FWeaponClassTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABaseWeapon> Weapon;
	
};

USTRUCT(BlueprintType)
struct FArmorInfoTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DEF;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EArmorType ArmorType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
};

UINTERFACE(MinimalAPI)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};


class MELEE_API IItemInterface
{
	GENERATED_BODY()

	
public:
};
