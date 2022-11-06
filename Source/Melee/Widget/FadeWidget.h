#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FadeWidget.generated.h"

class UWidgetAnimation;

UCLASS()
class MELEE_API UFadeWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	UWidgetAnimation* FadeIn;
	
public:
	void Fade(bool Boolean);
};
