#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/DataTable.h"
#include "MonsterInfoInterface.generated.h"



USTRUCT(BlueprintType)
struct FMonsterInfoTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MId = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name = TEXT("");
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHP = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ATK = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DEF = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemId = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemId2 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemId3 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Exp = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Gold = 0;
};


UINTERFACE(MinimalAPI)
class UMonsterInfoInterface : public UInterface
{
	GENERATED_BODY()
};

class MELEE_API IMonsterInfoInterface
{
	GENERATED_BODY()

public:
};
