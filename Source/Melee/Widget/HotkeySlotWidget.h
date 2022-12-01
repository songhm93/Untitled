#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HotkeySlotWidget.generated.h"


class UMaterialInstanceDynamic;
class UMaterialInstance;
class UImage;
class ABaseCharacter;

UCLASS()
class MELEE_API UHotkeySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInstanceDynamic* CooldownDynamicMaterialInst;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* CooldownMaterialInst;

	void SetCooldownPercentageScalar();

	UPROPERTY(BlueprintReadOnly, Meta = (BindWidget))
	UImage* Cooldown;

	UPROPERTY(BlueprintReadOnly)
	ABaseCharacter* MeleeCharacter;

protected:
	virtual void NativeConstruct() override;
	
};
