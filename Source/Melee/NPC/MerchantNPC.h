#pragma once

#include "CoreMinimal.h"
#include "BaseNPC.h"
#include "../Interface/InventoryInterface.h"
#include "../Interface/ItemInterface.h"
#include "MerchantNPC.generated.h"


UCLASS()
class MELEE_API AMerchantNPC : public ABaseNPC
{
	GENERATED_BODY()
	
public:
	AMerchantNPC();
	virtual void Interact(AActor* Caller) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TArray<int32> HaveItemId;

public:
	FORCEINLINE TArray<int32> GetHaveItemId() const { return HaveItemId; }

};
