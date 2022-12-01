#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "QuestInterface.generated.h"

UINTERFACE(MinimalAPI)
class UQuestInterface : public UInterface
{
	GENERATED_BODY()
};


class MELEE_API IQuestInterface
{
	GENERATED_BODY()

public:
	virtual int32 DeathMob(int32 MId) = 0;
	virtual void PlusCollectCurrentNum(int32 Amount) = 0;
};
