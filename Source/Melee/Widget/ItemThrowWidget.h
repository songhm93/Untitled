#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Interface/InventoryInterface.h"
#include "ItemThrowWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;
class UEditableTextBox;
class UInventoryComponent;

UCLASS()
class MELEE_API UItemThrowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(FItemInfoInSlot ItemInfoInSlot,int32 SlotIndex);
	void Update();

protected:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(Meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* NameText;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* NumText;

	UPROPERTY(Meta = (BindWidget))
	UButton* ThrowButton;

	UPROPERTY(Meta = (BindWidget))
	UButton* CancelButton;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* InputTextBox;

	UPROPERTY()
	UInventoryComponent* InventoryComp;

	FItemInfoInSlot ItemInfo;

	int32 Index;

	int32 ThrowNum;

	UFUNCTION()
	void CancelButtonClicked();

	UFUNCTION()
	void ThrowButtonClicked();

	UFUNCTION()
	void ReceiveThrowNum(const FText& Text);
	

};
