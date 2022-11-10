#include "InventoryComponent.h"

#include "../Item/MasterItem.h"
#include "../Equipment/DualWeapon.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	TotalSlotNum = 20;
	MaxStackSize = 99;
	CurrentSlotNum = 0;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	Slots.Reserve(TotalSlotNum);
	if(BaseWeapon)
	{
		Slots.Add(FInventorySlot(Cast<AMasterItem>(BaseWeapon.GetDefaultObject()), 1));
	}
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}



//AddItem을 몬스터 잡았을 때, 상점에서 샀을 때 필요,
//하나의 아이템의 총 갯수를 갖고 있음. 그래서 그걸 토대로 스택사이즈를 넘어서면 해당하는 자리만큼 차지하게 해서
//인벤토리 현 칸수를 알고 있음.
bool UInventoryComponent::AddItem(int32 ItemId, int32 Amount) //성공시 true 반환, 가득차서 못넣는 경우 false.
{
	FString ItemInfoTablePath = FString(TEXT("/Game/CombatSystem/DataTable/ItemInfo"));
	UDataTable* ItemInfoTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *ItemInfoTablePath));
	if(ItemInfoTableObject)
	{
		FItemInfoTable* ItemRow = nullptr;
		ItemRow = ItemInfoTableObject->FindRow<FItemInfoTable>(FName(FString::FromInt(ItemId)), TEXT(""));
		if(ItemRow)
		{
			if(CurrentSlotNum < TotalSlotNum) //인벤 공간 있음
			{
				if(ItemRow->Canstack == 1) //겹쳐지는 아이템이면
				{
					for(auto Inven : PlayerInventory) //현재 인벤토리를 쭉 돌면서 넣으려는 아이템을 찾아야함. 갯수가 필요함
					{
						if(Inven.Itemid == ItemRow->Itemid) //이미 인벤토리에 있는 아이템이면
						{
							if(Inven.Num > MaxStackSize) //이미 99개를 넘어선 상태면
							{
								int32 RegisteredSlotNum = Inven.Num / MaxStackSize; // 이미 차지하고 있던 칸 수
								if(Inven.Num % MaxStackSize != 0) ++RegisteredSlotNum;

								return CalculateInventory(Inven.Num, Amount, RegisteredSlotNum);
								
							}
							else //99개 아래로 가지고 있으면
							{
								int RegisteredSlotNum = 1; //이미 차지하고 있던 칸 수는 99개 아래니까 무조건 1 

								return CalculateInventory(Inven.Num, Amount, RegisteredSlotNum);
							}
						}
						else //인벤토리에 없는 아이템이면
						{
							if(Amount > MaxStackSize) //인벤토리에 새로 넣어야하는데 최대 스택사이즈를 넘으면
							{
								int32 RemainSlot = TotalSlotNum - CurrentSlotNum; //남는 인벤토리 칸 수 2
								int32 NeedToSlot = Amount % MaxStackSize;
								if(Inven.Num % MaxStackSize != 0) ++NeedToSlot; 
								if(NeedToSlot > RemainSlot)
								{
									return false;
								}
								else
								{
									CurrentSlotNum += NeedToSlot;
									return true;
								}
							}
							else //넘지 않으면
							{
								++CurrentSlotNum;
								return true;
							}
						}
					}
				}
				else //겹쳐지지 않는 아이템이면
				{
					int32 RemainSlot = TotalSlotNum - CurrentSlotNum;
					if(Amount > RemainSlot)
					{
						return false;
					}
					else
					{
						++CurrentSlotNum;
						return true;
					}
				}
			}
			else //인벤 공간 없음
			{
				return false;
			}
		}
	}
	return false;
}

bool UInventoryComponent::CalculateInventory(int32 ItemNum, int32 Amount, int32 RegisteredSlotNum)
{
	int32 AddedNumber = ItemNum + Amount; //합쳐질 총 갯수
	int32 RemainSlot = TotalSlotNum - CurrentSlotNum; //남는 칸 수

	int32 NeedToSlot = AddedNumber % MaxStackSize; //필요한 칸 수
	if(ItemNum % MaxStackSize != 0) ++NeedToSlot; 

	NeedToSlot -= RegisteredSlotNum; //필요한 칸수에서 이미 차지하고 있던 칸수는 뺌
	if(NeedToSlot > NeedToSlot) //남는 칸보다 필요한 칸이 많으면
	{
		return false; //실패
	}
	else
	{
		CurrentSlotNum += NeedToSlot;
		return true;
	}
}

void UInventoryComponent::AddGold()
{
	
}

void UInventoryComponent::InitInventory(TArray<FPlayerInventory> Inventory)
{
	PlayerInventory = Inventory;
	/*
	무기를 가져왔을 때 얘는 액터인데 그걸 어떻게 가져와야할까?
	isactor가 true면 ActorDataTable에서 블프를 가져오는? 이거는 나중에 인벤창 만들고,
	인벤 가져와서 무기를 골라서 장착하는 기능을 만들면 그때 필요.
	Equiped DB가 있어야함
	*/
	if(!PlayerInventory.IsEmpty())
	{
		FString ItemInfoTablePath = FString(TEXT("/Game/CombatSystem/DataTable/ItemInfo"));
		UDataTable* ItemInfoTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *ItemInfoTablePath));
		if(ItemInfoTableObject)
		{
			FItemInfoTable* ItemRow = nullptr;
			for(auto Inven : PlayerInventory)
			{
				ItemRow = ItemInfoTableObject->FindRow<FItemInfoTable>(FName(FString::FromInt(Inven.Itemid)), TEXT(""));
				if(ItemRow)
				{
					if(ItemRow->Canstack == 0) //겹쳐지는게 아니면
					{
						CurrentSlotNum += Inven.Num;
					}
					else //겹쳐지는거면
					{
						if(Inven.Num > MaxStackSize) //최대 겹치는걸 넘어가면
						{
							int32 NeedToSlot = Inven.Num / MaxStackSize;
							CurrentSlotNum += NeedToSlot;
							NeedToSlot = Inven.Num % MaxStackSize;
							if(NeedToSlot != 0) ++CurrentSlotNum;
						}
						else
						{
							++CurrentSlotNum;
						}
					}
					// if(ItemRow->Isactor == 1) //액터면.. 원래 이렇게 할게 아니고, 여기는 Init이라 위에까지가 끝. Equip DB가 필요함.
					// {
					// 	FString EquimentTablePath = FString(TEXT("/Game/CombatSystem/DataTable/EquipmentTable"));
					// 	UDataTable* EquimentTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *EquimentTablePath));
					// 	if(EquimentTableObject)
					// 	{
					// 		FEquipmentTable* EquipmentRow = nullptr;
					// 		EquipmentRow = ItemInfoTableObject->FindRow<FEquipmentTable>(FName(FString::FromInt(Inven.Itemid)), TEXT(""));
					// 		if(EquipmentRow)
					// 		{
					// 			//액터 블루프린트를 얻어온다. 장착할게 아니고 모든 블루프린트를 얻어올텐데.
					// 		}
					// 	}
					// }
				}
			}
		}
	}
}	