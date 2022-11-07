#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AMasterItem;
class ACharacter;

USTRUCT()
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	AMasterItem* Item;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 Amount;

};	

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:	
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 AmountOfSlot;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 MaxStackSize;

	UPROPERTY()
	ACharacter* OwnerCharacter;

	TArray<FInventorySlot> Slots;

	bool IsSlotEmpty(int32 Index);
};
