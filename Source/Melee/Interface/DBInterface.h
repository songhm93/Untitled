#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DBInterface.generated.h"


USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()
	UPROPERTY()
	bool Isvalid = false; //코틀린에서 DB접근할때 컬럼명은 다 소문자로, 여기서 DB접근할때 컬럼명은 시작만 대문자로...
	UPROPERTY()
	int32 Pid = -1;
	UPROPERTY()
	float Currenthp = 0.f;
	UPROPERTY()
	float Maxhp = 0.f;
	UPROPERTY()
	float Currentstamina = 0.f;
	UPROPERTY()
	float Maxstamina = 0.f;
	UPROPERTY()
	float Xcoord = 0.f;
	UPROPERTY()
	float Ycoord = 0.f;
	UPROPERTY()
	float Zcoord = 0.f;
	UPROPERTY()
	int32 Level = 1;
	UPROPERTY()
	float Currentexp = 0.f;
	UPROPERTY()
	float Maxexp = 0.f;
	UPROPERTY()
	float Atk = 0.f;
	UPROPERTY()
	float Def = 0.f;
	UPROPERTY()
	int32 Gold = 0.f;
};

USTRUCT(BlueprintType)
struct FMonsterInfo
{
	GENERATED_BODY()
	UPROPERTY()
	int32 Mid = 0;
	UPROPERTY()
	FString Name = TEXT("");
	UPROPERTY()
	float Maxhp = 0.f;
	UPROPERTY()
	float Atk = 0.f;
	UPROPERTY()
	float Def = 0.f;
	UPROPERTY()
	FString Itemid = TEXT("");
	UPROPERTY()
	FString Itemid2 = TEXT("");
	UPROPERTY()
	FString Itemid3 = TEXT("");
	UPROPERTY()
	float Exp = 0.f;
	UPROPERTY()
	int32 Gold = 0;
	
};

USTRUCT(BlueprintType)
struct FPlayerInventory
{
	GENERATED_BODY()
	UPROPERTY()
	int32 Inventorynum = 0;
	UPROPERTY()
	int32 Pid = 0;
	UPROPERTY()
	int32 Itemid = 0;
	UPROPERTY()
	int32 Num = 0;
	
	
};

UINTERFACE(MinimalAPI)
class UDBInterface : public UInterface
{
	GENERATED_BODY()
};


class MELEE_API IDBInterface
{
	GENERATED_BODY()

	
public:
};
