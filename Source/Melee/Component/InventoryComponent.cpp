#include "InventoryComponent.h"

#include "../Item/MasterItem.h"
#include "../Equipment/DualWeapon.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	AmountOfSlot = 10;
	MaxStackSize = 99;
	Gold = 0;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	Slots.Reserve(AmountOfSlot);
	if(BaseWeapon)
	{
		Slots.Add(FInventorySlot(Cast<AMasterItem>(BaseWeapon.GetDefaultObject()), 1));
	}
	
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UInventoryComponent::IsSlotEmpty(int32 Index)
{
	return !(Slots.IsValidIndex(Index));
}

FInventorySlot UInventoryComponent::GetSlotItem(int32 Index)
{
	if(Slots.IsValidIndex(Index))
	{
		return Slots[Index];
	}

	return FInventorySlot();
}

int32 UInventoryComponent::GetEmptySlot() //빈 슬롯이 있으면 해당하는 인덱스 반환, 없으면 -1 반환
{
	for(int32 i = 0; i < AmountOfSlot; ++i)
	{
		if(!Slots.IsValidIndex(i))
			return i;
	}

	return -1;
}

int32 UInventoryComponent::CanStackThisItem(AMasterItem* Item) //슬롯에 이미 있어서, 쌓을 수 있으면 해당 인덱스 반환.
{
	if(!Item) return -1;

	for(int32 i = 0; i < AmountOfSlot; ++i)
	{
		if(Slots.IsValidIndex(i))
		{
			if(Slots[i].Item->GetItemInfo().Name == Item->GetItemInfo().Name)
			{
				if(Slots[i].Amount < MaxStackSize)
				{
					return i;
				}
			}
		}
	}
	return -1;
}

//AddItem을 몬스터 잡았을 때, 상점에서 샀을 때 필요,
bool UInventoryComponent::AddItem(AMasterItem* Item, int32 Amount) //성공시 true 반환, 가득차서 못넣는 경우 false.
{
	if(Item)
	{
		if(Item->GetItemInfo().bCanbeStacked) //아이템이 스택이 되는 아이템이면
		{
			if(CanStackThisItem(Item) == -1) //슬롯에 없거나 갯수 때문에 쌓을 수 없음(빈 슬롯에 쌓아야 함)
			{
				int32 EmptySlotIdx = GetEmptySlot();
				if(EmptySlotIdx == -1) //빈 슬롯이 없음
				{
					return false;
				}
				else //빈 슬롯이 있음
				{
					Slots[EmptySlotIdx] = FInventorySlot(Item, Amount);
					return true;
				}
			}
			else //이미 슬롯에 있어서 쌓을 수 있음
			{
				int32 CanStackSize = MaxStackSize - Slots[CanStackThisItem(Item)].Amount; //쌓을 수 있는 양
				if(Amount <= CanStackSize) //쌓으면 됨
				{
					Slots[CanStackThisItem(Item)].Amount += Amount;
					return true;
				}
				else //가능한만큼 쌓고, 남는 양은 나머지 슬롯에 쌓아야 함
				{
					int32 LeftAmount = Amount - CanStackSize; //쌓고 남는 양
					Slots[CanStackThisItem(Item)].Amount += CanStackSize; //가능한 양만 쌓음
					if(GetEmptySlot() == -1) //빈슬롯이 없음
					{
						return false;
					}
					else
					{
						Slots[GetEmptySlot()] = FInventorySlot(Item, LeftAmount);
						return true;
					}
				}
			}
		}
		else //아이템이 스택이 안되는 아이템이면, 새로운 슬롯을 찾아야 함
		{
			int32 EmptySlotIdx = GetEmptySlot();
			if(EmptySlotIdx == -1) //빈 슬롯이 없음
			{
				return false;
			}
			else //빈 슬롯이 있음
			{
				for(int i = 0; i < Amount; ++i)
				{
					EmptySlotIdx = GetEmptySlot();
					if(EmptySlotIdx == -1) return false;
					Slots[EmptySlotIdx] = FInventorySlot(Item, 1);
				}
				return true;
			}
		}
	}
	return false;
}

int32 UInventoryComponent::GetAmountInSlot(int32 Index)
{
	int32 Amount = 0;
	if(Slots.IsValidIndex(Index))
	{
		Amount = Slots[Index].Amount;
	}
	return Amount;
}

void UInventoryComponent::AddGold()
{
	
}