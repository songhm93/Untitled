#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Interface/InventoryInterface.h"
#include "InventoryWidget.generated.h"


class UInventoryComponent;
class UButton;
class UInventorySlotWidget;
class UWidgetComponent;
class UUniformGridPanel;
class UUserWidget;

UCLASS()
class MELEE_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void GenerateSlotWidgets();
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
private:
	UPROPERTY(Meta = (BindWidget))
	UButton* CloseButton;

	UPROPERTY()
	UInventoryComponent* InventoryComp;

	int32 SlotIndex; //인벤 컴포넌트에 InventorySlots 인덱스가 곧 이 변수.

	FItemInfoInSlot ItemInfoInSlot;

	int32 SlotsPerRow;

	UPROPERTY(Meta = (BindWidget))
	UUniformGridPanel* SlotPanel;

	TArray<UUserWidget*> SlotWidgets;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInventorySlotWidget> InventorySlotWidgetClass;

	UFUNCTION()
	void VisibleWidget(bool Visible);

	UFUNCTION()
	void CloseInventory();

public:
	FORCEINLINE void SetInventorySlotWidget();
	
};
