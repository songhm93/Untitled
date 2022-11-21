#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Interface/InventoryInterface.h"
#include "MerchantInventorySlotWidget.generated.h"

class UTextBlock;
class UImage;
class UButton;
class UInfoWidget;


UCLASS()
class MELEE_API UMerchantInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(FItemInfoInSlot ItemInfoInSlot, int32 SlotIndex);
	void UpdateSlot(bool IsValid);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess ="true"))
	int32 Index;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	FItemInfoInSlot ItemInfo;

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ItemAmount;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* XText;

	UPROPERTY(Meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UButton* SlotButton;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ItemEquipped;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInfoWidget> InfoWidgetClass;

	UPROPERTY()
	UInfoWidget* InfoWidget;
};
