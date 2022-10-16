#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Type/Types.h"
#include "StateManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateBegin, ECharacterState, CharacterState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateEnd, ECharacterState, CharacterState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionBegin, ECharacterAction, CharacterAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionEnd, ECharacterAction, CharacterAction); //여기도 나중에 정리. 굳이 다이나믹, 멀티캐스트를 쓸 필요가 없을듯
DECLARE_DELEGATE_OneParam(FOnSprint, bool);

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

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	ECharacterState CurrentState;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	ECharacterAction CurrentAction;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	EMovementType MovementType;
public:	//get
	FORCEINLINE ECharacterState GetCurrentState() const { return CurrentState; }
	FORCEINLINE ECharacterAction GetCurrentAction() const { return CurrentAction; }
	FORCEINLINE EMovementType GetMovementType() const { return MovementType; }
public: //set

public:
	void SetCurrentState(ECharacterState State);
	void SetCurrentAction(ECharacterAction Action);
	bool IsCurrentStateEqualToThis(TArray<ECharacterState> StatesToCheck);
	bool IsCurrentActionEqualToThis(TArray<ECharacterAction> StatesToCheck);
	void ResetState();
	void ResetAction();
	void SetMovementType(EMovementType Type);
		
};
