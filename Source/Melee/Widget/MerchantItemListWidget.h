#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "../Interface/InventoryInterface.h"
#include "MerchantItemListWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;
class UItemBuyWidget;



UCLASS()
class MELEE_API UMerchantItemListWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	void Init(FItemInfoInSlot Info);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly,  Meta = (AllowPrivateAccess = "true"))
	UImage* ItemIcon;
	
	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly,  Meta = (AllowPrivateAccess = "true"))
	UTextBlock* ItemName;

	UPROPERTY(Meta = (BindWidget), BlueprintReadOnly,  Meta = (AllowPrivateAccess = "true"))
	UTextBlock* BuyGold;

	UPROPERTY(Meta = (BindWidget))
	UButton* BuyButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  Meta = (AllowPrivateAccess = "true"))
	FItemInfoInSlot ItemInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UItemBuyWidget> ItemBuyWidgetClass;

};
