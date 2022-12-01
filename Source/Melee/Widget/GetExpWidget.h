#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GetExpWidget.generated.h"

class UWidgetAnimation;
class UTextBlock;
class UHorizontalBox;
class ABaseCharacter;

UCLASS()
class MELEE_API UGetExpWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void FadeAnim(int32 ExpValue);
	
	UPROPERTY(Meta = (BindWidget))
	UHorizontalBox* GetExpBox;

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	UWidgetAnimation* Fade;
	
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* ExpPoint;

	UPROPERTY()
	ABaseCharacter* Player;
	
	FTimerHandle RemoveWidgetTimerHandle;

	float RemoveWidgetTime;

	void RemoveWidget();

public:

};
