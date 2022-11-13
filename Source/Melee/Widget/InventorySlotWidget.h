#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Interface/InventoryInterface.h"
#include "InventorySlotWidget.generated.h"


class UTextBlock;
class UImage;
class UInventoryComponent;
class UButton;

UCLASS()
class MELEE_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(FItemInfoInSlot ItemInfoInSlot, int32 SlotIndex);
	void UpdateSlot(bool IsValid);

	

	int32 Index; //인벤 컴포넌트에 InventorySlots 인덱스가 곧 이 변수.

	FItemInfoInSlot ItemInfo;

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ItemAmount;

	UPROPERTY(Meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(Meta = (BindWidget))
	UButton* SlotButton;
	
	
};
