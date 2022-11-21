#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Interface/InventoryInterface.h"
#include "InventoryActionMenuWidget.generated.h"

class UButton;
class UTextBlock;
class UInventoryComponent;
class UItemThrowWidget;

UCLASS()
class MELEE_API UInventoryActionMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(FItemInfoInSlot ItemInfoInSlot, int32 SlotIndex);

protected:
	virtual void NativeConstruct() override;
private:
	UPROPERTY(Meta = (BindWidget))
	UButton* UseButton;

	UPROPERTY(Meta = (BindWidget))
	UButton* ThrowButton;

	UPROPERTY(Meta = (BindWidget))
	UButton* DevideButton;

	UPROPERTY(Meta = (BindWidget))
	UButton* CancelButton;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* UseText;

	FItemInfoInSlot ItemInfo;

	UPROPERTY()
	UInventoryComponent* InventoryComp;

	int32 Index;

	void Update();

	UFUNCTION()
	void UseButtonClicked();

	UFUNCTION()
	void CancelButtonClicked();

	UFUNCTION()
	void DevideButtonClicked();

	UFUNCTION()
	void ThrowButtonClicked();
public:
	
	
};
