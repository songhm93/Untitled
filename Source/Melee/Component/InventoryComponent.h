#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Interface/ItemInterface.h"
#include "../Interface/DBInterface.h"
#include "../Interface/InventoryInterface.h"
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
class MELEE_API UInventoryComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()

public:	
	UInventoryComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual bool AddItem(int32 ItemId, int32 Amount) override;
	virtual void AddGold(int32 GoldAmount) override;
protected:
	virtual void BeginPlay() override;
	
private:	
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 TotalSlotNum; //인벤토리 슬롯 갯수

	int CurrentSlotNum;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	TArray<FPlayerInventory> PlayerInventory;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 MaxStackSize; //쌓이는 아이템, 얼마나 쌓이는 지.

	UPROPERTY()
	ACharacter* OwnerCharacter;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	TArray<FInventorySlot> Slots;//삭제할듯

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ADualWeapon> BaseWeapon; // 기본적으로 넣을 무기.

	bool CalculateInventory(int32 ItemNum, int32 Amount, int32 RegisteredSlotNum);


	//인벤토리 DB에서 게임 시작할 때 가지고 있는 아이템 가져오고,
	//배열로 가져옴.

public:
	

public:
	FORCEINLINE TArray<FInventorySlot> GetInventorySlot() const { return Slots; }

public:
	void InitInventory(TArray<FPlayerInventory> Inventory);
};
