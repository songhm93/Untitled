#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Type/Stats.h"
#include "StatBarTextWidget.generated.h"

class UTextBlock;
class UStatsComponent;

UCLASS()
class MELEE_API UStatBarTextWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	EStats StatType;
	UPROPERTY()
	UStatsComponent* StatComp;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* StatNumber;
	UFUNCTION()
	void OnStatValueUpdated(EStats Stat, float Value);
	void Init();
};
