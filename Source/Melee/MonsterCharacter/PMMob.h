#pragma once

#include "CoreMinimal.h"
#include "PreemptiveMonster.h"
#include "PMMob.generated.h"

class UWidgetComponent;

UCLASS()
class MELEE_API APMMob : public APreemptiveMonster
{
	GENERATED_BODY()

public:
	APMMob();
	virtual void OnTargeted(bool IsTargeted) override;
	virtual void Dead() override;
protected:
	virtual void BeginPlay() override;

private:
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	// UWidgetComponent* HPBarWidget;

	//void HideHPBar();

	// void HPBarOnOff(bool Show);

	// FTimerHandle HideHPBarTimerHandle;

	// float HideHPBarTime;
	
};
