#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Interface/InventoryInterface.h"
#include "ItemBuyWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;
class UEditableTextBox;
class UInventoryComponent;

UCLASS()
class MELEE_API UItemBuyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	

protected:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UImage* ItemIcon;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UTextBlock* NameText;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UTextBlock* Desc;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UButton* BuyButton;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UButton* CancelButton;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UEditableTextBox* InputTextBox;

	UPROPERTY()
	UInventoryComponent* InventoryComp;

	FItemInfoInSlot ItemInfo;
	
};
