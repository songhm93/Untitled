#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Type/Types.h"
#include "StateManagerComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStateBegin, ECurrentState);
DECLARE_DELEGATE_OneParam(FShouldRegen, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCombatState, bool);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_API UStateManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStateManagerComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FOnStateBegin OnStateBegin;
	FShouldRegen ShouldRegen;
	FOnCombatState OnCombatState;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	ECurrentState CurrentState;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	ECurrentAction CurrentAction;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	EMovementType CurrentMovementType;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	ECurrentCombatState CurrentCombatState;

	bool IsHPPotionCooldown;

	bool IsStaminaPotionCooldown;

	FTimerHandle HPPotionTimerHandle;

	FTimerHandle StaminaPotionTimerHandle;

	float HPPotionCooldownTime;

	float StaminaPotionCooldownTime;

	FTimerDelegate PotionCooldownDelegate;

	bool IsDodgeCooldown;

	FTimerHandle DodgeTimerHandle;

	float DodgeCooldownTime;

	UFUNCTION()
	void PotionCooldownComplete(bool IsHPPotion);

	void DodgeCooldownComplete();
	
public:	//get
	FORCEINLINE ECurrentState GetCurrentState() const { return CurrentState; }
	FORCEINLINE ECurrentAction GetCurrentAction() const { return CurrentAction; }
	FORCEINLINE EMovementType GetMovementType() const { return CurrentMovementType; }
	FORCEINLINE bool GetIsHPPotionCooldown() const { return IsHPPotionCooldown; }
	FORCEINLINE bool GetIsStaminaPotionCooldown() const { return IsStaminaPotionCooldown; }
	FORCEINLINE bool GetIsDodgeCooldown() const { return IsDodgeCooldown; }
public:
	void SetCurrentState(ECurrentState State);
	void SetCurrentAction(ECurrentAction Action);
	bool IsCurrentStateEqualToThis(TArray<ECurrentState> StatesToCheck);
	bool IsCurrentActionEqualToThis(TArray<ECurrentAction> StatesToCheck);
	void ResetState();
	void ResetAction();
	void SetMovementType(EMovementType Type);
	void SetCurrentCombatState(ECurrentCombatState CombatState);
	ECurrentCombatState GetCurrentCombatState();
	void SetPotionCooldown(bool IsHPPotion);
	void SetDodgeCoolDown();
		
};
