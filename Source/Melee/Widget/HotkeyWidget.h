#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HotkeyWidget.generated.h"

class UHotkeySlotWidget;

UCLASS()
class MELEE_API UHotkeyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget), Meta = (AllowPrivateAccess = "true"))
	UHotkeySlotWidget* HotkeySlot;

public:
	FORCEINLINE UHotkeySlotWidget* GetHotkeySlot() const { return HotkeySlot; }
	
};
