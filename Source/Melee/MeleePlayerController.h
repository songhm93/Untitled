#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MeleePlayerController.generated.h"

class AMeleeCharacter;
//class UUserWidget;

UCLASS()
class MELEE_API AMeleePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMeleePlayerController();
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
protected:
	virtual void SetupInputComponent() override;
private:
	void LightAttackPressed();
	void LightAttackReleased();
	void TrackingChargedAttack(float DeltaTime);
	bool bLeftClickIsPressed;
	float LeftClickTime;
	float ChargedTime;
	UPROPERTY()
	AMeleeCharacter* MeleeCharacter;
	bool bCharged;

	void TrackingSprint();
	
};
