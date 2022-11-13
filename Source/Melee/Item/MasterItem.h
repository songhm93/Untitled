#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Type/ItemCategory.h"
#include "MasterItem.generated.h"


UCLASS()
class MELEE_API AMasterItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AMasterItem();
	virtual void Tick(float DeltaTime) override;
    virtual void UseItem(int32 ItemId, int32 SlotIndex);
protected:
	virtual void BeginPlay() override;

private:	


public:
    

public:


};
