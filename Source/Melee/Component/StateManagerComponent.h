#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Type/Types.h"
#include "StateManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateBegin, ECurrentState, CharacterState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateEnd, ECurrentState, CharacterState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionBegin, ECurrentAction, CharacterAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionEnd, ECurrentAction, CharacterAction); //여기도 나중에 정리. 굳이 다이나믹, 멀티캐스트를 쓸 필요가 없을듯
DECLARE_DELEGATE_OneParam(FOnSprint, bool);
DECLARE_DELEGATE_OneParam(FOnCombatState, bool);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_API UStateManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStateManagerComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FOnStateBegin OnStateBegin;
	FOnStateEnd OnStateEnd;
	FOnActionBegin OnActionBegin;
	FOnActionEnd OnActionEnd;
	FOnSprint OnSprint;
	FOnCombatState OnCombatState;
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	ECurrentState CurrentState;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	ECurrentAction CurrentAction;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	EMovementType MovementType;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	ECurrentCombatState CurrentCombatState;
public:	//get
	FORCEINLINE ECurrentState GetCurrentState() const { return CurrentState; }
	FORCEINLINE ECurrentAction GetCurrentAction() const { return CurrentAction; }
	FORCEINLINE EMovementType GetMovementType() const { return MovementType; }
	
public: //set
	
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
		
};
