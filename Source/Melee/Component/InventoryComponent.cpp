#include "InventoryComponent.h"
#include "JsonObjectConverter.h"

#include "../Item/Consumeable.h"
#include "../Equipment/DualWeapon.h"
#include "../PlayerController/MeleePlayerController.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	TotalSlotNum = 50;
	MaxStackSize = 99;
	CurrentSlotNum = 0;
	bIsVisible = false;
	Http = &FHttpModule::Get();
	Gold = 0;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	MPC = Cast<AMeleePlayerController>(Cast<APawn>(GetOwner())->GetController());
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UInventoryComponent::InitInventory(TArray<FPlayerInventoryDB> Inventory)
{
	PlayerInventory = Inventory;
	
	InventorySlots.Reserve(TotalSlotNum);

	if(!PlayerInventory.IsEmpty())
	{
		FString ItemInfoTablePath = FString(TEXT("/Game/CombatSystem/DataTable/ItemInfo"));
		UDataTable* ItemInfoTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *ItemInfoTablePath));
		if(ItemInfoTableObject)
		{
			FItemInfoTable* ItemRow = nullptr;
			int Index = 0;
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
							InventorySlots.Add(FItemInfoInSlot(Inven.Itemid, 1, ItemRow->Name, ItemRow->Desc, ItemRow->Icon, ItemRow->Usetext, ItemRow->Canuse, ItemRow->Canstack, ItemRow->Category, ItemRow->Isactor, Inven.Equipped, ItemRow->BuyGold, ItemRow->SellGold)); //슬롯에 1개씩, 갯수만큼 넣음.
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
									InventorySlots.Add(FItemInfoInSlot(Inven.Itemid, MaxStackSize, ItemRow->Name, ItemRow->Desc, ItemRow->Icon, ItemRow->Usetext, ItemRow->Canuse, ItemRow->Canstack, ItemRow->Category, ItemRow->Isactor, Inven.Equipped, ItemRow->BuyGold, ItemRow->SellGold));
								}
								else
								{
									InventorySlots.Add(FItemInfoInSlot(Inven.Itemid, RemainNum, ItemRow->Name, ItemRow->Desc, ItemRow->Icon, ItemRow->Usetext, ItemRow->Canuse, ItemRow->Canstack, ItemRow->Category, ItemRow->Isactor, Inven.Equipped, ItemRow->BuyGold, ItemRow->SellGold));
								}
								RemainNum -= MaxStackSize;
							}
						}
						else
						{
							++CurrentSlotNum;
							InventorySlots.Add(FItemInfoInSlot(Inven.Itemid, Inven.Num, ItemRow->Name, ItemRow->Desc, ItemRow->Icon, ItemRow->Usetext, ItemRow->Canuse, ItemRow->Canstack, ItemRow->Category, ItemRow->Isactor, Inven.Equipped, ItemRow->BuyGold, ItemRow->SellGold));
						}
					}
					//Equipped 상태면 장착해야함
					EItemCategory Category = InventorySlots[Index].Category;
					bool Condition = 
					(ItemRow->Category == EItemCategory::HELMET) ||
					(ItemRow->Category == EItemCategory::CHEST) ||
					(ItemRow->Category == EItemCategory::GAUNTLET) ||
					(ItemRow->Category == EItemCategory::BOOT);
					if(InventorySlots[Index].Equipped)
					{
						if(Category == EItemCategory::WEAPON)
						{
							OnEquippedWeaponSpawn.ExecuteIfBound(Inven.Itemid);
						}
						else
						{
							OnEquippedArmorApply.ExecuteIfBound(Inven.Itemid);
						}
					}
				}
				++Index;
			}
			OnGenerateSlotWidget.ExecuteIfBound();
		}
	}
}	

//AddItem을 몬스터 잡았을 때, 상점에서 샀을 때 필요,
//InventorySlots와 PlayerInventory는 별개. 
//InventorySlots는 MaxStackSize만큼씩 나눠서 ItemInfo로 저장한 배열. 
//PlayerInventory는 DB에 들어가 있는 형태의 배열. 같은 ItemId로 들어가 있어도 갯수가 다르다.
bool UInventoryComponent::AddItem(int32 ItemId, int32 Amount, bool bFromMonster) //성공시 true 반환, 가득차서 못넣는 경우 false.
{
	if(Amount == 0) return false;
	FString ItemInfoTablePath = FString(TEXT("/Game/CombatSystem/DataTable/ItemInfo"));
	UDataTable* ItemInfoTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *ItemInfoTablePath));
	bool ExistItem = false;
	if(ItemInfoTableObject)
	{
		FItemInfoTable* ItemRow = nullptr;
		ItemRow = ItemInfoTableObject->FindRow<FItemInfoTable>(FName(FString::FromInt(ItemId)), TEXT(""));
		if(ItemRow)
		{
			if(TotalSlotNum > CurrentSlotNum) //인벤 공간 있음
			{
				FItemInfoInSlot AddItemInfo = FItemInfoInSlot(ItemId, Amount, ItemRow->Name, ItemRow->Desc, ItemRow->Icon, ItemRow->Usetext, ItemRow->Canuse, ItemRow->Canstack, ItemRow->Category, ItemRow->Isactor, 0, ItemRow->BuyGold, ItemRow->SellGold);
				if(ItemRow->Canstack == 1) //겹쳐지는 아이템이면
				{
					int32 TotalAmount = 0;
					int32 RegisteredSlotThisItem = 0; //이 아이템이 이미 차지하고 있는 슬롯의 갯수 

					for(auto Slot : InventorySlots) //현재 인벤토리를 쭉 돌면서 이미 있는지 찾는다. 갯수가 필요함.
					{
						if(Slot.ItemId == ItemId) //이미 인벤토리에 있는 아이템이면
						{
							++RegisteredSlotThisItem; //같은 아이템이라도, 99개씩 차있지 않아도 여러개로 분할되어있을 수 있음
							TotalAmount += Slot.Amount; //여러칸에 있는 아이템들 모조리 더하기. 
							ExistItem = true;
						}
					}

					if(ExistItem) //이미 존재하면
					{
						bool Success = AddAmountExistSlot(ItemRow, Amount, TotalAmount, RegisteredSlotThisItem);
						if(Success)
						{
							if(bFromMonster)
							{
								MPC = MPC == nullptr ? Cast<AMeleePlayerController>(Cast<APawn>(GetOwner())->GetController()) : MPC;
								if(MPC)
								{
									MPC->GetItemWidgetVisible(AddItemInfo, Amount);
								}
							}
							return true;
						}
						else
						{
							return false;
						}
						
					}
					else
					{
						if(Amount > MaxStackSize) //기존에 없어서 인벤토리에 새로 넣어야하는데 최대 스택사이즈를 넘으면
						{
							if(!InsertItemNewSlot(ItemRow, TotalSlotNum, Amount)) return false;
						}
						else //넘지 않으면
						{
							++CurrentSlotNum;
							InventorySlots.Add(AddItemInfo); //새로운 슬롯 InventorySlots Add
						}
						RequestAddItemInsert(ItemId, Amount); //DB Insert
						AddPlayerInventory(ItemId, Amount); //할때 DB와 똑같이 맞추기 위해 PlayerInventory도 Insert
						if(bFromMonster)
						{
							MPC = MPC == nullptr ? Cast<AMeleePlayerController>(Cast<APawn>(GetOwner())->GetController()) : MPC;
							if(MPC)
							{
								MPC->GetItemWidgetVisible(AddItemInfo, Amount);
							}
						}
							
						return true;
					}
				}
				else //겹쳐지지 않는 아이템이면
				{
					int32 RemainSlot = TotalSlotNum - CurrentSlotNum;
					if(Amount > RemainSlot) //넣으려는 갯수가 남는 슬롯보다 많으면
					{
						return false;
					}
					else
					{
						FItemInfoInSlot ItemInfo = FItemInfoInSlot(ItemId, 1, ItemRow->Name, ItemRow->Desc, ItemRow->Icon, ItemRow->Usetext, ItemRow->Canuse, ItemRow->Canstack, ItemRow->Category, ItemRow->Isactor, 0, ItemRow->BuyGold, ItemRow->SellGold);
						for(int i = 0; i < Amount; ++i)
						{
							++CurrentSlotNum;
							InventorySlots.Add(ItemInfo); //새로운 슬롯 InventorySlots Add. Amount만큼 늘어남
						}
						if(ItemRow->Category == EItemCategory::WEAPON || 
						ItemRow->Category == EItemCategory::HELMET ||
						ItemRow->Category == EItemCategory::CHEST ||
						ItemRow->Category == EItemCategory::GAUNTLET ||
						ItemRow->Category == EItemCategory::BOOT) //장비인 경우 DB inventory에도 한칸에 하나씩 들어가게.
						{
							for(int i = 0; i < Amount; ++i)
							{
								RequestAddItemInsert(ItemId, 1); //DB Insert
								AddPlayerInventory(ItemId, 1); //DB와 똑같이 맞추기 위해 PlayerInventory도 Insert
							}
							if(bFromMonster)
							{
								MPC = MPC == nullptr ? Cast<AMeleePlayerController>(Cast<APawn>(GetOwner())->GetController()) : MPC;
								if(MPC)
								{
									MPC->GetItemWidgetVisible(AddItemInfo, Amount);
								}
							}
								
							return  true;
						}
						else
						{	
							int i = 0;
							for(auto Inven : PlayerInventory) //현재 인벤토리를 쭉 돌면서 이미 있는지 찾는다.
							{
								if(Inven.Itemid == ItemId) //이미 인벤토리에 있는 아이템이면
								{
									RequestAddItemUpdateNum(ItemId, PlayerInventory[i].Num + Amount); //DB Update
									PlayerInventory[i].Num += Amount; //DB와 똑같이 맞추기 위해 PlayerInventory도 업데이트
									if(bFromMonster)
									{
										MPC = MPC == nullptr ? Cast<AMeleePlayerController>(Cast<APawn>(GetOwner())->GetController()) : MPC;
										if(MPC)
										{
											MPC->GetItemWidgetVisible(AddItemInfo, Amount);
										}
									}
									
									return true;
								}
								++i;
							}
							RequestAddItemInsert(ItemId, Amount); //DB Insert
							AddPlayerInventory(ItemId, Amount); //DB와 똑같이 맞추기 위해 PlayerInventory도 Insert
							if(bFromMonster)
							{
								MPC = MPC == nullptr ? Cast<AMeleePlayerController>(Cast<APawn>(GetOwner())->GetController()) : MPC;
								if(MPC)
								{
									MPC->GetItemWidgetVisible(AddItemInfo, Amount);
								}
							}

							return true;
						}
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
		RequestUseItemMinusAmount(SlotIndex, Amount);
		InventorySlots.RemoveAt(SlotIndex);
	}
	else
	{
		RequestUseItemMinusAmount(SlotIndex, Amount);
		InventorySlots[SlotIndex].Amount -= Amount;
	}
	int32 i = 0;
	for(auto Inven : PlayerInventory) //DB와 똑같이 맞추기 위해 PlayerInventory도 업데이트
	{
		if(Inven.Itemid == InventorySlots[SlotIndex].ItemId)
		{
			PlayerInventory[i].Num -= Amount;
			if(PlayerInventory[i].Num == 0)
			{
				RequestUseItemDelete(SlotIndex);
			}
			break;
		}
		++i;
	}

	OnGenerateSlotWidget.ExecuteIfBound();
	return true;
}

bool UInventoryComponent::SwapSlot(int32 Index1, int32 Index2)
{
	if(!InventorySlots.IsValidIndex(Index1) || !InventorySlots.IsValidIndex(Index2)) return false;
	if(InventorySlots[Index1].Category == InventorySlots[Index2].Category) return false;
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
	
	FItemInfoInSlot Temp = InventorySlots[DividedSlotIndex];
	InventorySlots.Add(Temp); //같은 정보 일단 복붙
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
		EItemCategory Category = InventorySlots[SlotIndex].Category;
		bool Condition = 
		(InventorySlots[SlotIndex].Category == EItemCategory::HELMET) ||
		(InventorySlots[SlotIndex].Category == EItemCategory::CHEST) ||
		(InventorySlots[SlotIndex].Category == EItemCategory::GAUNTLET) ||
		(InventorySlots[SlotIndex].Category == EItemCategory::BOOT); //방어구

		if(Category == EItemCategory::WEAPON)
		{
			int i = 0;
			for(auto Inven : InventorySlots) //장착되어있는 무기를 찾아야함
			{
				if(Inven.Category == Category) //찾음
				{
					InventorySlots[i].Equipped = 0;
					RequestItemUpdateEquipped(i, InventorySlots[i].Equipped);
				}
				++i;
			}
			
			//장착하려는 장비의 상태를 변경.
			InventorySlots[SlotIndex].Equipped = 1;
			RequestItemUpdateEquipped(SlotIndex, InventorySlots[SlotIndex].Equipped);
			OnEquipWeapon.ExecuteIfBound(InventorySlots[SlotIndex].ItemId);
			OnGenerateSlotWidget.ExecuteIfBound();
		}
		else if(Condition) //방어구
		{
			//장착하려는 장비의 상태를 변경.
			if(!InventorySlots[SlotIndex].Equipped)
				OnEquipArmor.ExecuteIfBound(InventorySlots[SlotIndex].ItemId);
		
			InventorySlots[SlotIndex].Equipped = !InventorySlots[SlotIndex].Equipped;
			RequestItemUpdateEquipped(SlotIndex, InventorySlots[SlotIndex].Equipped);
			OnGenerateSlotWidget.ExecuteIfBound();
		}
		else if(Category == EItemCategory::CONSUMEABLE) //소모품 효과 실행
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
}

void UInventoryComponent::DecreaseItemAcount(bool Success, int32 SlotIndex) //true면 1개 감소, false면 감소X
{
	if(!InventorySlots.IsValidIndex(SlotIndex)) return;
	if(Success)
	{
		RemoveItemAtSlot(SlotIndex, 1); //1개 제거
	}
}

void UInventoryComponent::RequestUseItemMinusAmount(int32 SlotIndex, int32 Amount) //Num -= 1
{
	FPlayerInventoryDB Inventory;
	Inventory.Pid = 9824;
	Inventory.Itemid = InventorySlots[SlotIndex].ItemId;
	Inventory.Num = Amount; //원래 Num은 갯수지만 여기선 쿼리문 이용해서 뺄 갯수로 json 보냄.

	if(Http)
    {
        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

        Request->SetURL("http://localhost:8080/api/PlayerInventory/MinusAmount/");
        Request->SetVerb("PUT");
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        
        FString JsonString;
        FJsonObjectConverter::UStructToJsonObjectString(Inventory, JsonString);
        Request->SetContentAsString(JsonString);

        Request->ProcessRequest();
    }
}

void UInventoryComponent::RequestUseItemDelete(int32 SlotIndex) //아이템 갯수가 0개 -> DB에서 삭제
{
	FPlayerInventoryDB Inventory;
	Inventory.Pid = 9824;
	Inventory.Itemid = InventorySlots[SlotIndex].ItemId;

	if(Http)
    {
        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

        Request->SetURL("http://localhost:8080/api/PlayerInventory/Delete/");
        Request->SetVerb("PUT");
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        
        FString JsonString;
        FJsonObjectConverter::UStructToJsonObjectString(Inventory, JsonString);
        Request->SetContentAsString(JsonString);

        Request->ProcessRequest();
    }
}

void UInventoryComponent::RequestAddItemUpdateNum(int32 ItemId, int32 Amount) //Amount를 Update
{
	FPlayerInventoryDB Inventory;
	Inventory.Pid = 9824;
	Inventory.Itemid = ItemId;
	Inventory.Num = Amount;
	
	if(Http)
    {
        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

        Request->SetURL("http://localhost:8080/api/PlayerInventory/AddItem/");
        Request->SetVerb("PUT");
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        
        FString JsonString;
        FJsonObjectConverter::UStructToJsonObjectString(Inventory, JsonString);
        Request->SetContentAsString(JsonString);

        Request->ProcessRequest();
    }
}

void UInventoryComponent::RequestAddItemInsert(int32 ItemId, int32 Amount) // 인벤토리에 없는 새로운 아이템일 때 Insert
{
	FPlayerInventoryDB Inventory;
	Inventory.Pid = 9824;
	Inventory.Itemid = ItemId;
	Inventory.Num = Amount;

	if(Http)
    {
        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

        Request->SetURL("http://localhost:8080/api/PlayerInventory/Insert/");
        Request->SetVerb("PUT");
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        
        FString JsonString;
        FJsonObjectConverter::UStructToJsonObjectString(Inventory, JsonString);
        Request->SetContentAsString(JsonString);

        Request->ProcessRequest();
    }
}

void UInventoryComponent::RequestItemUpdateEquipped(int32 SlotIndex, bool Equipped) //장착/해제한 장비 값 변경.
{
	FPlayerInventoryDB Inventory;
	Inventory.Pid = 9824;
	Inventory.Itemid = InventorySlots[SlotIndex].ItemId;
	Inventory.Equipped = Equipped;

	if(Http)
    {
        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

        Request->SetURL("http://localhost:8080/api/PlayerInventory/Equipped/");
        Request->SetVerb("PUT");
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        
        FString JsonString;
        FJsonObjectConverter::UStructToJsonObjectString(Inventory, JsonString);
        Request->SetContentAsString(JsonString);

        Request->ProcessRequest();
    }
}

void UInventoryComponent::AddGold(int32 GoldAmount, bool bFromMonster)
{
	Gold += GoldAmount;
	
	FString ItemInfoTablePath = FString(TEXT("/Game/CombatSystem/DataTable/ItemInfo"));
	UDataTable* ItemInfoTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *ItemInfoTablePath));
	if(ItemInfoTableObject)
	{
		FItemInfoTable* ItemRow = nullptr;
		ItemRow = ItemInfoTableObject->FindRow<FItemInfoTable>(FName(FString::FromInt(111111)), TEXT(""));
		if(ItemRow)
		{
			FItemInfoInSlot AddItemInfo = FItemInfoInSlot(ItemRow->Itemid, GoldAmount, ItemRow->Name, ItemRow->Desc, ItemRow->Icon, ItemRow->Usetext, ItemRow->Canuse, ItemRow->Canstack, ItemRow->Category, ItemRow->Isactor, 0, ItemRow->BuyGold, ItemRow->SellGold);
			if(bFromMonster)
			{
				MPC = MPC == nullptr ? Cast<AMeleePlayerController>(Cast<APawn>(GetOwner())->GetController()) : MPC;
				if(MPC)
				{
					MPC->GetItemWidgetVisible(AddItemInfo, GoldAmount);
				}
			}
		}
	}
	
}

void UInventoryComponent::AddPlayerInventory(int32 ItemId, int32 Amount)
{
	FPlayerInventoryDB InsertInventory;
	InsertInventory.Inventorynum = 0;
	InsertInventory.Pid = 9824;
	InsertInventory.Itemid = ItemId;
	InsertInventory.Num = Amount;
	InsertInventory.Equipped = 0;
	PlayerInventory.Add(InsertInventory);
}

bool UInventoryComponent::AddAmountExistSlot(FItemInfoTable* ItemRow, int32 Amount, int32 TotalAmount, int32 RegisteredSlotThisItem)
{
	int32 AddedAmount = TotalAmount + Amount; //만약 들어오게 됐을 때의 양
	int32 RemainSlot = TotalSlotNum - CurrentSlotNum; //남는 칸 수 
	int32 NeedToSlot = AddedAmount / MaxStackSize; //필요한 칸 수 
	if(AddedAmount % MaxStackSize != 0) ++NeedToSlot; 
	int32 SlotNum = NeedToSlot; //돌면서 채워야하는 칸 수

	NeedToSlot = NeedToSlot - RegisteredSlotThisItem; //필요한 칸수에서 이미 차지하고 있던 칸수는 뺌
	if(NeedToSlot > RemainSlot) //남는 칸보다 필요한 칸이 많으면
	{
		return false; //못 넣음
	}
	else //여기선 넣으려는 아이템이 이미 존재하는 슬롯은 갱신, 그렇지 않은 슬롯은 추가. 삭제하는 경우도 있음.
	{
		CurrentSlotNum += NeedToSlot;

		int i = 0;
		int32 RemainAmount = AddedAmount; //갱신 후 남은 양
		TArray<int32> RemoveIdx;
		bool ShouldRemoveSlot = false;
		for(auto Slot : InventorySlots) //이미존재하는 슬롯갯수가 NeedToSlot보다 많을땐 그 갯수 차이만큼 삭제가 되야함
		{
			if(Slot.ItemId == ItemRow->Itemid)
			{
				if(RemainAmount == 0) //칸을 모두 갱신했는데 차지하고 있던 칸이 남은 경우엔 해당 슬롯 삭제
				{
					RemoveIdx.Add(i);
					ShouldRemoveSlot = true;
				}
				if((AddedAmount >= MaxStackSize) && (SlotNum > 0))
				{
					InventorySlots[i].Amount = MaxStackSize;
					AddedAmount -= MaxStackSize;
				}
				else if((AddedAmount < MaxStackSize) && (SlotNum > 0))
				{
					InventorySlots[i].Amount = AddedAmount;
				}
				if(RemainAmount != 0)
				{
					RemainAmount -= InventorySlots[i].Amount;
				}
				--SlotNum;
			}
			++i;
		}
		if(ShouldRemoveSlot)
			ReverseRemoveSlotIndex(RemoveIdx);
		if(RemainAmount > 0) //존재하던 슬롯을 다 돌았는데 갱신하고 남은 양이 여전히 남았으면 슬롯에 새로 추가
		{
			for(int j = 0; j < SlotNum; ++j)
			{
				if(RemainAmount > MaxStackSize)
				{
					InventorySlots.Add(FItemInfoInSlot(ItemRow->Itemid, MaxStackSize, ItemRow->Name, ItemRow->Desc, ItemRow->Icon, ItemRow->Usetext, ItemRow->Canuse, ItemRow->Canstack, ItemRow->Category, ItemRow->Isactor, 0, ItemRow->BuyGold, ItemRow->SellGold));
					RemainAmount -= MaxStackSize;
				}
				else
				{
					InventorySlots.Add(FItemInfoInSlot(ItemRow->Itemid, RemainAmount, ItemRow->Name, ItemRow->Desc, ItemRow->Icon, ItemRow->Usetext, ItemRow->Canuse, ItemRow->Canstack, ItemRow->Category, ItemRow->Isactor, 0, ItemRow->BuyGold, ItemRow->SellGold));
				}
			}
		}
	}
	
	int i = 0;
	for(auto Inven : PlayerInventory)
	{
		if(Inven.Itemid == ItemRow->Itemid)
		{
			RequestAddItemUpdateNum(ItemRow->Itemid, PlayerInventory[i].Num + Amount); //DB Update
			PlayerInventory[i].Num += Amount; //DB와 똑같이 맞추기 위해 PlayerInventory도 업데이트
		}
		++i;
	}

	return true;
}

bool UInventoryComponent::InsertItemNewSlot(FItemInfoTable* ItemRow, int32 TotalSlotNumber, int32 Amount)
{
	int32 RemainSlot = TotalSlotNumber - CurrentSlotNum; //남는 인벤토리 칸 수
	int32 NeedToSlot = Amount / MaxStackSize;
	if(Amount % MaxStackSize != 0) ++NeedToSlot; 
	if(NeedToSlot > RemainSlot) //남는 칸보다 필요한 칸이 많으면
	{
		return false;
	}
	else
	{
		CurrentSlotNum += NeedToSlot;
		int32 RemainNum = Amount;

		for(int32 j = 0; j < NeedToSlot; ++j)//새로운 슬롯 InventorySlots Add. NeedToSlot만큼 늘어남.
		{
			if(RemainNum > MaxStackSize)
			{
				InventorySlots.Add(FItemInfoInSlot(ItemRow->Itemid, MaxStackSize, ItemRow->Name, ItemRow->Desc, ItemRow->Icon, ItemRow->Usetext, ItemRow->Canuse, ItemRow->Canstack, ItemRow->Category, ItemRow->Isactor, 0, ItemRow->BuyGold, ItemRow->SellGold));
			}
			else
			{
				InventorySlots.Add(FItemInfoInSlot(ItemRow->Itemid, RemainNum, ItemRow->Name, ItemRow->Desc, ItemRow->Icon, ItemRow->Usetext, ItemRow->Canuse, ItemRow->Canstack, ItemRow->Category, ItemRow->Isactor, 0, ItemRow->BuyGold, ItemRow->SellGold));
			}
			RemainNum -= MaxStackSize;
		}
	}
	return true;
}

void UInventoryComponent::MergeSameItemEachSlot(int32 MoveIndex, int32 FixIndex) //겹쳐지는 같은 아이템 합치기
{
	if(!InventorySlots.IsValidIndex(MoveIndex) || !InventorySlots.IsValidIndex(FixIndex)) return;
	if((InventorySlots[MoveIndex].ItemId != InventorySlots[FixIndex].ItemId) || !InventorySlots[MoveIndex].CanStack) return;
	
	if(InventorySlots[MoveIndex].Amount + InventorySlots[FixIndex].Amount > MaxStackSize) //합치는데 MaxStackSize보다 큰 경우
	{
		InventorySlots[MoveIndex].Amount = InventorySlots[MoveIndex].Amount - (MaxStackSize - InventorySlots[FixIndex].Amount);
		InventorySlots[FixIndex].Amount = MaxStackSize;
	}
	else if(InventorySlots[MoveIndex].Amount + InventorySlots[FixIndex].Amount <= MaxStackSize) //작거나 같은 경우. 
	{
		InventorySlots[FixIndex].Amount  += InventorySlots[MoveIndex].Amount;
		InventorySlots.RemoveAt(MoveIndex);
	}
	OnGenerateSlotWidget.ExecuteIfBound();
}

void UInventoryComponent::ReverseRemoveSlotIndex(TArray<int32> IndexArray)
{
	for(int i = IndexArray.Num() - 1; i >= 0; --i) //
	{
		InventorySlots.RemoveAt(IndexArray[i]);
	}
}

FItemInfoInSlot UInventoryComponent::GetItemInfoInSlot(int32 Index)
{
	if(!InventorySlots.IsValidIndex(Index)) return FItemInfoInSlot();

	return InventorySlots[Index];
}