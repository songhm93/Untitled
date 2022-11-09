#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Type/Stats.h"
#include "StatBarWidget.generated.h"

class UProgressBar;
class UStatsComponent;

UCLASS()
class MELEE_API UStatBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

private:
	UPROPERTY(Meta = (BindWidget))
	UProgressBar* StatBar;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	EStats StatType;
	UPROPERTY()
	UStatsComponent* StatComp;
	UFUNCTION()
	void OnStatValueUpdated(EStats Stat, float Value);
	void Init();
};
