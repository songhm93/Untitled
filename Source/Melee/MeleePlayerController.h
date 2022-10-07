#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MeleePlayerController.generated.h"

class AMeleeCharacter;

UCLASS()
class MELEE_API AMeleePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMeleePlayerController();
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
protected:
	virtual void SetupInputComponent() override;
private:
	void LightAttackPressed();
	void LightAttackReleased();
	void TrackChargedAttack(float DeltaTime);
	bool bLeftClickIsPressed;
	float LeftClickTime;
	float ChargedTime;
	AMeleeCharacter* Character;
	bool bCharged;
};
