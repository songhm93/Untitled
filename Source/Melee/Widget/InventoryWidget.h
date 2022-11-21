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
class UInventoryActionMenuWidget;
class UScrollBox;

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

	UPROPERTY(Meta = (BindWidget))
	UScrollBox* InventoryScrollBox;

	UPROPERTY()
	UInventoryComponent* InventoryComp;

	FItemInfoInSlot ItemInfoInSlot;

	int32 SlotsPerRow;

	UPROPERTY(Meta = (BindWidget))
	UUniformGridPanel* SlotPanel;

	TArray<UUserWidget*> SlotWidgets;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInventorySlotWidget> InventorySlotWidgetClass;

	UPROPERTY(Meta = (BindWidget))
	UInventoryActionMenuWidget* InventoryActionMenuWidget;

	UFUNCTION()
	void VisibleWidget(bool Visible);

	UFUNCTION()
	void CloseInventory();

	UFUNCTION(BlueprintCallable)
	void SlotClicked(int32 SlotIndex);

	void HideThrowWidget();
public:
	FORCEINLINE TArray<UUserWidget*> GetSlotWidgets() const { return SlotWidgets; }
	
};
