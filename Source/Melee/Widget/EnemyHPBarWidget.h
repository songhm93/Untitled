#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Type/Stats.h"
#include "EnemyHPBarWidget.generated.h"


class UProgressBar;
class UStatsComponent;


UCLASS()
class MELEE_API UEnemyHPBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(UStatsComponent* Comp);
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(Meta = (BindWidget))
	UProgressBar* EnemyHPBar;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	EStats StatType;
	UPROPERTY()
	UStatsComponent* StatComp;
	UFUNCTION()
	void OnStatValueUpdated(EStats Stat, float Value);
	
	
};
