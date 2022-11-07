#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	AmountOfSlot = 6;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Slots.Reserve(AmountOfSlot);
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UInventoryComponent::IsSlotEmpty(int32 Index)
{
	if(Slots.IsValidIndex(Index))
		return false;

	return true;
}
