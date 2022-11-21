#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Interface/InventoryInterface.h"
#include "GetItemWidget.generated.h"

class UWidgetAnimation;
class UTextBlock;
class UImage;
class UHorizontalBox;

UCLASS()
class MELEE_API UGetItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(FItemInfoInSlot ItemInfoInSlot, int32 Amount);

	void FadeIn();

	void FadeOut();

	void Update();

	void EndFadeOutAnim();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	FItemInfoInSlot ItemInfo;

	UPROPERTY(Meta = (BindWidget))
	UHorizontalBox* GetItemWidgetBox;
private:
	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInAnim;

	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutAnim;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ItemAmount;

	UPROPERTY(Meta = (BindWidget))
	UImage* ItemIcon;

	int32 Amount;

	FTimerHandle FadeOutTimerHandle;

	float FadeOutTime = 2.f;

	FTimerHandle EndFadeOutAnimTimerHandle;
};
