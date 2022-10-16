#include "StateManagerComponent.h"

#include "../Type/Types.h"

UStateManagerComponent::UStateManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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

void UStateManagerComponent::ResetAction()
{
	SetCurrentAction(ECharacterAction::NOTHING);
}

bool UStateManagerComponent::IsCurrentStateEqualToThis(TArray<ECharacterState> StatesToCheck)
{
	return StatesToCheck.Contains(CurrentState);
}

void UStateManagerComponent::SetCurrentAction(ECharacterAction Action)
{
	if(CurrentAction != Action)
	{
		OnActionEnd.Broadcast(CurrentAction);
		CurrentAction = Action;
		OnActionBegin.Broadcast(CurrentAction);
	}
}

bool UStateManagerComponent::IsCurrentActionEqualToThis(TArray<ECharacterAction> StatesToCheck)
{
	return StatesToCheck.Contains(CurrentAction);
}

void UStateManagerComponent::SetMovementType(EMovementType Type)
{
	MovementType = Type;
	if (MovementType == EMovementType::SPRINTING)
	{
		OnSprint.ExecuteIfBound(false);
	}
	else
	{
		OnSprint.ExecuteIfBound(true);
	}
	
}