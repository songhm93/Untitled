#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Type/Stats.h"
#include "EnemyHPBarWidget.generated.h"


class UProgressBar;
class UStatsComponent;
class UTextBlock;
class UWidgetAnimation;

UCLASS()
class MELEE_API UEnemyHPBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(UStatsComponent* Comp);
	void PlayOpacityAnim(bool Forward);
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

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* MonsterName;

	UFUNCTION()
	void OnStatValueUpdated(EStats Stat, float Value);

	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	UWidgetAnimation* OpacityAnim;
	
public: //get
	FORCEINLINE UTextBlock* GetMonsterNameText() const { return MonsterName; };

public:
	void SetMonsterName(FString Name);
	
};
