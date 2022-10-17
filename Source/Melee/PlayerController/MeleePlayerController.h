#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MeleePlayerController.generated.h"


DECLARE_DELEGATE(FOnLightAttack);
DECLARE_DELEGATE(FOnChargedAttack);

class ABaseCharacter;

UCLASS()
class MELEE_API AMeleePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMeleePlayerController();

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;

	FOnLightAttack OnLightAttack;
	
	FOnChargedAttack OnChargedAttack;

protected:
	virtual void SetupInputComponent() override;

private:
	void AttackButtonPressed();

	void AttackButtonReleased();

	void TrackingChargedAttack(float DeltaTime);

	void TrackingSprint();

	UPROPERTY()
	ABaseCharacter* BaseCharacter;

	bool bLeftClickIsPressed;

	float LeftClickTime;

	float ChargedTime;

	bool bCharged;
	
};
