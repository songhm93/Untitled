#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types.h"
#include "StateManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateBegin, ECharacterState, CharacterState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateEnd, ECharacterState, CharacterState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionBegin, ECharacterAction, CharacterAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionEnd, ECharacterAction, CharacterAction);

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
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	ECharacterState CurrentState;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	ECharacterAction CurrentAction;
public:	//get
	FORCEINLINE ECharacterState GetCurrentState() const { return CurrentState; }
	FORCEINLINE ECharacterAction GetCurrentAction() const { return CurrentAction; }
public: //set

public:
	void SetCurrentState(ECharacterState State);
	void SetCurrentAction(ECharacterAction Action);
	bool IsCurrentStateEqualToThis(TArray<ECharacterState> StatesToCheck);
	bool IsCurrentActionEqualToThis(TArray<ECharacterAction> StatesToCheck);
	void ResetState();
	void ResetAction();
		
};
