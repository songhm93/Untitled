#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types.h"
#include "StateManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateBegin, ECharacterState, CharacterState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateEnd, ECharacterState, CharacterState);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_API UStateManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStateManagerComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FOnStateBegin OnStateBegin;
	FOnStateEnd OnStateEnd;
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	ECharacterState CurrentState;
	
public:	//get
	FORCEINLINE ECharacterState GetCurrentState() const { return CurrentState; }
public: //set

public:
	void SetCurrentState(ECharacterState State);
	bool IsCurrentStateEqualToThis(TArray<ECharacterState> StatesToCheck);
	void ResetState();
		
};
