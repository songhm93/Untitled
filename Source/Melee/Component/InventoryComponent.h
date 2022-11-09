#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AMasterItem;
class ACharacter;
class ADualWeapon;

USTRUCT()
struct FInventorySlot
{
	GENERATED_BODY()
	FInventorySlot(){}
	FInventorySlot(AMasterItem* _Item, int32 _Amount)
	{
		Item = _Item;
		Amount = _Amount;
	}

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
	bool AddItem(AMasterItem* Item, int32 Amount);
	void AddGold();
protected:
	virtual void BeginPlay() override;
	
private:	
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 AmountOfSlot; //인벤토리 슬롯 갯수

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 MaxStackSize; //쌓이는 아이템, 얼마나 쌓이는 지.

	UPROPERTY()
	ACharacter* OwnerCharacter;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	TArray<FInventorySlot> Slots;

	bool IsSlotEmpty(int32 Index);

	FInventorySlot GetSlotItem(int32 Index);

	int32 GetEmptySlot();

	int32 CanStackThisItem(AMasterItem* Item);

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ADualWeapon> BaseWeapon; // 기본적으로 넣을 무기.

	int32 GetAmountInSlot(int32 Index);

	int32 Gold;

public:
	FORCEINLINE TArray<FInventorySlot> GetInventorySlot() const { return Slots; }
};
