#include "InventoryComponent.h"

#include "../Item/Consumeable.h"
#include "../Equipment/DualWeapon.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	TotalSlotNum = 50;
	MaxStackSize = 99;
	CurrentSlotNum = 0;
	bIsVisible = false;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventorySlots.Reserve(TotalSlotNum);
	
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

//AddItem을 몬스터 잡았을 때, 상점에서 샀을 때 필요,
//하나의 아이템의 총 갯수를 갖고 있음. 그래서 그걸 토대로 스택사이즈를 넘어서면 해당하는 자리만큼 차지하게 해서
//인벤토리 현 칸수를 알고 있음.
//이걸 호출했을 때 InventoryWidget에 GenerateSlotWidgets함수를 다시 호출해야할듯
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
			if(TotalSlotNum > CurrentSlotNum) //인벤 공간 있음
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
								int32 RemainSlot = TotalSlotNum - CurrentSlotNum; //남는 인벤토리 칸 수
								int32 NeedToSlot = Amount / MaxStackSize;
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

void UInventoryComponent::AddGold(int32 GoldAmount)
{
	
}

void UInventoryComponent::InitInventory(TArray<FPlayerInventory> Inventory)
{
	PlayerInventory = Inventory;
	
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
						for(int i = 0; i < Inven.Num; ++i)
						{
							InventorySlots.Add(FItemInfoInSlot(Inven.Itemid, 1, ItemRow->Name, ItemRow->Desc, ItemRow->Icon, ItemRow->Usetext, ItemRow->Canuse, ItemRow->Canstack, ItemRow->Category, ItemRow->Isactor)); //슬롯에 1개씩, 갯수만큼 넣음.
						}
					}
					else //겹쳐지는거면
					{
						if(Inven.Num > MaxStackSize) //최대 겹치는걸 넘어가면
						{
							int32 NeedToSlot = Inven.Num / MaxStackSize;
							CurrentSlotNum += NeedToSlot;
							if(Inven.Num % MaxStackSize != 0)
							{
								++CurrentSlotNum;
								++NeedToSlot;
							} 
							int32 RemainNum = Inven.Num;

							for(int i = 0; i < NeedToSlot; ++i)
							{
								if(RemainNum > MaxStackSize)
								{
									InventorySlots.Add(FItemInfoInSlot(Inven.Itemid, MaxStackSize, ItemRow->Name, ItemRow->Desc, ItemRow->Icon, ItemRow->Usetext, ItemRow->Canuse, ItemRow->Canstack, ItemRow->Category, ItemRow->Isactor));
								}
								else
								{
									InventorySlots.Add(FItemInfoInSlot(Inven.Itemid, RemainNum, ItemRow->Name, ItemRow->Desc, ItemRow->Icon, ItemRow->Usetext, ItemRow->Canuse, ItemRow->Canstack, ItemRow->Category, ItemRow->Isactor));
								}
								RemainNum -= MaxStackSize;
							}
						}
						else
						{
							++CurrentSlotNum;
							InventorySlots.Add(FItemInfoInSlot(Inven.Itemid, Inven.Num, ItemRow->Name, ItemRow->Desc, ItemRow->Icon, ItemRow->Usetext, ItemRow->Canuse, ItemRow->Canstack, ItemRow->Category, ItemRow->Isactor));
						}
					}
				}
			}
			OnGenerateSlotWidget.ExecuteIfBound();
		}
	}
}	

void UInventoryComponent::VisibleInventory(bool Visible)
{
	OnVisibleInventory.ExecuteIfBound(Visible);
	bIsVisible = Visible;
}

bool UInventoryComponent::RemoveItemAtSlot(int32 SlotIndex, int32 Amount)
{
	if(!InventorySlots.IsValidIndex(SlotIndex) || Amount == 0) return false;
	if(Amount > InventorySlots[SlotIndex].Amount)
	{
		return false;
	} 
	else if(Amount == InventorySlots[SlotIndex].Amount)
	{
		InventorySlots.RemoveAt(SlotIndex);
		OnGenerateSlotWidget.ExecuteIfBound();
	}
	else
	{
		InventorySlots[SlotIndex].Amount -= Amount;
		OnGenerateSlotWidget.ExecuteIfBound();
	}
	return true;
}

bool UInventoryComponent::SwapSlot(int32 Index1, int32 Index2)
{
	if(!InventorySlots.IsValidIndex(Index1) || !InventorySlots.IsValidIndex(Index2)) return false;
	FItemInfoInSlot Temp = InventorySlots[Index1];
	InventorySlots[Index1] = InventorySlots[Index2];
	InventorySlots[Index2] = Temp;
	OnGenerateSlotWidget.ExecuteIfBound();
	return true;
}

bool UInventoryComponent::DevideStack(int32 DividedSlotIndex, int32 DevideAmount)
{
	if(!InventorySlots.IsValidIndex(DividedSlotIndex)) return false;
	if(InventorySlots[DividedSlotIndex].CanStack == false || DevideAmount > InventorySlots[DividedSlotIndex].Amount) return false;
	int32 EmptySlotIndex = FindEmptySlotIndex();
	if(EmptySlotIndex == -1) return false;
	InventorySlots.Insert(InventorySlots[DividedSlotIndex], EmptySlotIndex); //빈 슬롯 인덱스에 같은 정보 넣고
	InventorySlots[EmptySlotIndex].Amount = DevideAmount; //갯수만 나눌 갯수로 바꿔줌
	InventorySlots[DividedSlotIndex].Amount -= DevideAmount; //나눠진 슬롯의 갯수는 나눈 걸 뺀 갯수로 변경.
	OnGenerateSlotWidget.ExecuteIfBound();
	return true;
}

int32 UInventoryComponent::FindEmptySlotIndex() //빈 슬롯이 없으면 -1 리턴
{
	for(int i = 0; i < TotalSlotNum; ++i)
	{
		if(!InventorySlots.IsValidIndex(i)) return i;
	}
	return -1;
}

void UInventoryComponent::UseItemAtIndex(int32 SlotIndex)
{
	if(!InventorySlots.IsValidIndex(SlotIndex)) return;

	//사용가능한 아이템인가? YES -> 장비인가? -> Yes -> 무기인지 방어구인지 보고 스폰
	//사용가능한 아이템인가? YES -> 장비인가? -> No -> 무슨 아이템인지 보고 효과 실행
	//사용가능한 아이템인가? No  -> 사용 안됨.(퀘스트템 같은거)(갖다줬을 때 사용되는거)
	if(InventorySlots[SlotIndex].CanUse)
	{
		
		if(InventorySlots[SlotIndex].Category == EItemCategory::EQUIPMENT) //무기 방어구
		{
			OnEquipWeapon.ExecuteIfBound(InventorySlots[SlotIndex].ItemId);
		}
		else if(InventorySlots[SlotIndex].Category == EItemCategory::CONSUMEABLE)//소모품 효과 실행
		{
			FActorSpawnParameters Params; 
			Params.Owner = GetOwner();
			AConsumeable* Item = GetWorld()->SpawnActor<AConsumeable>(AConsumeable::StaticClass(), FTransform(), Params);
			if(Item)
			{
				Item->UseSuccess.BindUObject(this, &ThisClass::DecreaseItemAcount);
				Item->UseItem(InventorySlots[SlotIndex].ItemId, SlotIndex);
			}
		}
	}
	else
	{
		return;
	}
}

void UInventoryComponent::DecreaseItemAcount(bool Success, int32 SlotIndex) //true면 1개 감소, false면 감소X
{
	if(!InventorySlots.IsValidIndex(SlotIndex)) return;
	if(Success)
	{
		--InventorySlots[SlotIndex].Amount;
		OnGenerateSlotWidget.ExecuteIfBound();
	}
}