#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Interface/InventoryInterface.h"
#include "DraggedItemWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class MELEE_API UDraggedItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void Init(FItemInfoInSlot ItemInfoInSlot, int32 SlotIndex);
	void UpdateSlot();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess ="true"))
	int32 Index;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	FItemInfoInSlot ItemInfo;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ItemAmount;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* XText;

	UPROPERTY(Meta = (BindWidget))
	UImage* ItemIcon;
	
};
