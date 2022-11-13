#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

class UWidgetComponent;
class UInventoryWidget;

UCLASS()
class MELEE_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
private:
	//UPROPERTY(Meta = (BindWidget))
	//UInventoryWidget* InventoryWidget;

public:
	//FORCEINLINE UInventoryWidget* GetInventoryWidget() const { return InventoryWidget; }

};
