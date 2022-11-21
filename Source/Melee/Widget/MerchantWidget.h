#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Interface/InventoryInterface.h"
#include "MerchantWidget.generated.h"

class UButton;
class UListView;
class UMerchantItemListWidget;
class UUniformGridPanel;
class UMerchantInventorySlotWidget;
class UScrollBox;
class UInventoryComponent;
class UTextBlock;


UCLASS()
class MELEE_API UMerchantWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(TArray<FItemInfoInSlot> Info);
	void Update();

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(Meta = (BindWidget))
	UButton* BuyViewButton;

	UPROPERTY(Meta = (BindWidget))
	UButton* ExitButton;

	UPROPERTY(Meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UListView* ItemListView;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TArray<FItemInfoInSlot> ItemsInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMerchantItemListWidget> ItemListWidgetClass;

	int32 SlotsPerRow;

	UPROPERTY(Meta = (BindWidget))
	UUniformGridPanel* SlotPanel;

	TArray<UUserWidget*> SlotWidgets;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMerchantInventorySlotWidget> InventorySlotWidgetClass;

	UPROPERTY(Meta = (BindWidget))
	UScrollBox* InventoryScrollBox;

	UPROPERTY()
	UInventoryComponent* InventoryComp;

	UFUNCTION()
	void ExitMerchant();

	UFUNCTION(BlueprintCallable)
	void GenerateInventorySlots();

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* CurrentGold;

};
