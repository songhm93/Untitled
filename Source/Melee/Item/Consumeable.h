#pragma once

#include "CoreMinimal.h"
#include "MasterItem.h"
#include "Consumeable.generated.h"


DECLARE_DELEGATE_TwoParams(FUseSuccess, bool, int32);


UCLASS()
class MELEE_API AConsumeable : public AMasterItem
{
	GENERATED_BODY()

public:	
	AConsumeable();
	virtual void Tick(float DeltaTime) override;
    virtual void UseItem(int32 ItemId, int32 SlotIndex) override;
	FUseSuccess UseSuccess;
	

private:
	bool UsePotion(bool IsHPPotion);
};
