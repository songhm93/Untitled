#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Interface/ItemInterface.h"
#include "../Interface/DBInterface.h"
#include "../Interface/InventoryInterface.h"
#include "Http.h"
#include "InventoryComponent.generated.h"

class AMasterItem;
class ADualWeapon;
class AMeleePlayerController;

DECLARE_DELEGATE(FOnGenerateSlotWidget);
DECLARE_DELEGATE_OneParam(FOnVisibleInventory, bool);
DECLARE_DELEGATE_OneParam(FOnEquipWeapon, int32);
DECLARE_DELEGATE_OneParam(FOnEquippedWeaponSpawn, int32);
DECLARE_DELEGATE_OneParam(FOnEquipArmor, int32);
DECLARE_DELEGATE_OneParam(FOnEquippedArmorApply, int32);
DECLARE_DELEGATE_OneParam(FUnequipArmor, EItemCategory);
DECLARE_DELEGATE(FOnGenerateMerchantSlot);



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_API UInventoryComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()

public:	
	UInventoryComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable)
	virtual bool AddItem(int32 ItemId, int32 Amount, bool bFromMonster) override;
	UFUNCTION(BlueprintCallable)
	virtual void AddGold(int32 GoldAmount, bool bFromMonster) override;
	FOnGenerateSlotWidget OnGenerateSlotWidget;
	FOnVisibleInventory OnVisibleInventory;
	FOnEquipWeapon OnEquipWeapon;
	FOnEquipArmor OnEquipArmor;
	FUnequipArmor UnequipArmor;
	FOnEquippedWeaponSpawn OnEquippedWeaponSpawn;
	FOnEquippedArmorApply OnEquippedArmorApply;
	UFUNCTION(BlueprintCallable)
	void InventoryUpdate();
	void OnInvenNumRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
	void OnAddEquipmentInsertComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
	FOnGenerateMerchantSlot OnGenerateMerchantSlot;
protected:
	virtual void BeginPlay() override;
	FHttpModule* Http;

private:	
	void DecreasePlayerInventory(int32 SlotIndex, int32 Amount);

	TArray<FPlayerInventoryDB> ConvertToPlayerInventory(const FString& ResponseString);

	int32 FindEmptySlotIndex();

	void ReverseRemoveSlotIndex(TArray<int32> Index);

	UFUNCTION()
	void DecreaseItemAcount(bool Success, int32 SlotIndex);

	UPROPERTY()
	AMeleePlayerController* MPC;
	
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 TotalSlotNum; //???????????? ?????? ??????

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TArray<FPlayerInventoryDB> PlayerInventory; //PlayerInventory PID??? ????????? DB ??????

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 MaxStackSize; //????????? ?????????, ????????? ????????? ???.

	TArray<FItemInfoInSlot> InventorySlots; //?????? ??????????????? ?????? ????????????.

	int32 CurrentSlotNum;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	TArray<FPlayerInventoryDB> GetInventoryNum; //?????? Insert ??? DB?????? ????????? InventoryNum??? ????????? ??????

	bool bIsVisible; //???????????? ??? Visible

	void RequestDeleteEquipment(int32 ItemId, int32 InventoryNum);

	void RequestUseItemMinusAmount(int32 SlotIndex, int32 Amount); //????????? ?????? -> Num ????????????

	void RequestAddItemUpdateNum(int32 ItemId, int32 Amount);

	void RequestAddItemInsert(int32 ItemId, int32 Amount);

	void RequestAddEquipmentInsert(int32 ItemId, bool IsLast);

	void RequestUseItemDelete(int32 SlotIndex);

	void RequestItemUpdateEquipped(int32 SlotIndex, bool Equipped);

	void RequestGetInventoryNum(int32 ItemId, int32 Amount);

	void AddPlayerInventory(int32 ItemId, int32 Amount);

	void AddEquipmentPlayerInventory(int32 ItemId, int32 Amount);

	bool AddAmountExistSlot(FItemInfoTable* ItemRow, int32 Amount, int32 TotalAmount, int32 RegisteredSlotThisItem);

	bool InsertItemNewSlot(FItemInfoTable* ItemRow, int32 TotalSlotNum, int32 Amount);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	int32 Gold;

	int32 EquipmentAmount; //?????? ??????????????? ?????????

	int32 EquipmentId;

	FItemInfoInSlot EquipmentInfo;
public:
	FORCEINLINE TArray<FItemInfoInSlot> GetInventorySlots() const { return InventorySlots; }
	FORCEINLINE int32 GetTotalSlotNum() const { return TotalSlotNum; }
	FORCEINLINE int32 GetCurrentSlotNum() const { return CurrentSlotNum; }
	FORCEINLINE bool GetIsVisible() const { return bIsVisible; }
	FORCEINLINE int32 GetMaxStackSize() const { return MaxStackSize; }
	FORCEINLINE int32 GetGold() const { return Gold; }
	
public:
	FORCEINLINE void SetIsVisible(bool Boolean) { bIsVisible = Boolean; }
	FORCEINLINE void InitGold(int32 Value) { Gold = Value; }

public:
	void InitInventory(TArray<FPlayerInventoryDB> Inventory);
	void VisibleInventory(bool Visible);
	UFUNCTION(BlueprintCallable)
	void UseItemAtIndex(int32 SlotIndex);
	UFUNCTION(BlueprintCallable)
	bool SwapSlot(int32 Index1, int32 Index2);
	UFUNCTION(BlueprintCallable)
	bool DevideStack(int32 SlotIndex, int32 DevideAmount);
	bool RemoveItemAtSlot(int32 SlotIndex, int32 Amount);
	UFUNCTION(BlueprintCallable)
	void MergeSameItemEachSlot(int32 MoveIndex, int32 FixIndex);
	UFUNCTION(BlueprintPure)
	FItemInfoInSlot GetItemInfoInSlot(int32 Index);
	UFUNCTION(BlueprintPure)
	int32 GetItemTotalAmount(int32 ItemId);
	
};
