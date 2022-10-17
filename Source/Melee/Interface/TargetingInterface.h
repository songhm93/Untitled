#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TargetingInterface.generated.h"

UINTERFACE(MinimalAPI)
class UTargetingInterface : public UInterface
{
	GENERATED_BODY()
};

class MELEE_API ITargetingInterface
{
	GENERATED_BODY()

public:
	virtual bool CanBeTargeted() = 0;
	virtual void OnTargeted(bool IsTargeted) = 0;

};
