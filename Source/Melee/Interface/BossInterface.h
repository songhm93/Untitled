#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BossInterface.generated.h"


UINTERFACE(MinimalAPI)
class UBossInterface : public UInterface
{
	GENERATED_BODY()
};

class MELEE_API IBossInterface
{
	GENERATED_BODY()

	
public:
	virtual void Special1() = 0;
	virtual void Special2() = 0;
	virtual void Special3() = 0;
	virtual void Special4() = 0;
	virtual void Special5() = 0;
};
