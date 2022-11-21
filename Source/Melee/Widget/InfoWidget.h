#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Interface/InventoryInterface.h"
#include "InfoWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS()
class MELEE_API UInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void Init(FItemInfoInSlot ItemInfoInSlot);
	
private:
	void UpdateInfo();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	FItemInfoInSlot ItemInfo;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Category;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* Info;

	UPROPERTY(Meta = (BindWidget))
	UImage* ItemIcon;
};
