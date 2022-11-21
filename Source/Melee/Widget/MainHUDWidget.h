#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

class UWidgetComponent;
class UInventoryWidget;
class UItemThrowWidget;
class UGetItemWidget;

UCLASS()
class MELEE_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
private:
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UInventoryWidget* InventoryWidget;

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UItemThrowWidget* ItemThrowWidget;

	

public:
	FORCEINLINE UItemThrowWidget* GetItemThrowWidget() const { return ItemThrowWidget; }
	FORCEINLINE UInventoryWidget* GetInventoryWidget() const { return InventoryWidget; }

};
