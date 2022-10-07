#include "StateManagerComponent.h"
#include "Types.h"

UStateManagerComponent::UStateManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStateManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStateManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UStateManagerComponent::SetCurrentState(ECharacterState State)
{
	if(CurrentState != State)
	{
		OnStateEnd.Broadcast(CurrentState);
		CurrentState = State;
		OnStateBegin.Broadcast(CurrentState);
	}
}


void UStateManagerComponent::ResetState()
{
	SetCurrentState(ECharacterState::NOTHING);
}

bool UStateManagerComponent::IsCurrentStateEqualToThis(TArray<ECharacterState> StatesToCheck)
{
	return StatesToCheck.Contains(CurrentState);
}
