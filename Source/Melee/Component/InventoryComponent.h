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

DECLARE_DELEGATE(FOnGenerateSlotWidget);
DECLARE_DELEGATE_OneParam(FOnVisibleInventory, bool);
DECLARE_DELEGATE_OneParam(FOnEquipWeapon, int32);

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
	FOnGenerateSlotWidget OnGenerateSlotWidget;
	FOnVisibleInventory OnVisibleInventory;
	FOnEquipWeapon OnEquipWeapon;
	void UseItemAtIndex(int32 SlotIndex);//private으로 옮기기. 테스트하려고 public
	bool SwapSlot(int32 Index1, int32 Index2);
protected:
	virtual void BeginPlay() override;
	
private:	
	bool CalculateInventory(int32 ItemNum, int32 Amount, int32 RegisteredSlotNum);

	bool RemoveItemAtSlot(int32 SlotIndex, int32 Amount);

	

	bool DevideStack(int32 SlotIndex, int32 SplitAmount);

	int32 FindEmptySlotIndex();

	UFUNCTION()
	void DecreaseItemAcount(bool Success, int32 SlotIndex);
	
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 TotalSlotNum; //인벤토리 슬롯 갯수

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	TArray<FPlayerInventory> PlayerInventory;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 MaxStackSize; //쌓이는 아이템, 얼마나 쌓이는 지.

	UPROPERTY()
	ACharacter* OwnerCharacter;

	TArray<FItemInfoInSlot> InventorySlots; //인벤 한칸한칸에 뭐가 들었는지.

	int32 CurrentSlotNum;

	bool bIsVisible; //인벤토리 창 Visible

public:
	FORCEINLINE TArray<FItemInfoInSlot> GetInventorySlots() const { return InventorySlots; }
	FORCEINLINE int32 GetTotalSlotNum() const { return TotalSlotNum; }
	FORCEINLINE int32 GetCurrentSlotNum() const { return CurrentSlotNum; }
	FORCEINLINE bool GetIsVisible() const { return bIsVisible; }
public:
	FORCEINLINE void SetIsVisible(bool Boolean) {  bIsVisible = Boolean; }
public:
	void InitInventory(TArray<FPlayerInventory> Inventory);
	void VisibleInventory(bool Visible);
};
