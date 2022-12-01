#include "StateManagerComponent.h"

#include "../Type/Types.h"
#include "CombatComponent.h"

UStateManagerComponent::UStateManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentMovementType = EMovementType::JOGGING;
	CurrentState = ECurrentState::NOTHING;
	CurrentAction = ECurrentAction::NOTHING;
	CurrentCombatState = ECurrentCombatState::NONE_COMBAT_STATE;
	IsHPPotionCooldown = false;
	IsStaminaPotionCooldown = false;
	HPPotionCooldownTime = 7.f;
	StaminaPotionCooldownTime = 7.f;
	IsDodgeCooldown = false;
	DodgeCooldownTime = 5.f;

}

void UStateManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner())
	{
		UCombatComponent* CombatComp = Cast<UCombatComponent>(GetOwner()->GetComponentByClass(UCombatComponent::StaticClass()));	
		if(CombatComp)
		{
			CombatComp->GetCurrentCombatState.BindUObject(this, &ThisClass::GetCurrentCombatState);
			CombatComp->GetCurrentState.BindUObject(this, &ThisClass::GetCurrentState);
			CombatComp->IsCurrentStateEqualToThis.BindUObject(this, &ThisClass::IsCurrentStateEqualToThis);
			CombatComp->OnUpdateCurrentState.BindUObject(this, &ThisClass::SetCurrentState);
			CombatComp->OnUpdateCurrentAction.BindUObject(this, &ThisClass::SetCurrentAction);
			CombatComp->GetCurrentMovementType.BindUObject(this, &ThisClass::GetMovementType);
		}
	}
}

void UStateManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UStateManagerComponent::SetCurrentState(ECurrentState State)
{
	if(CurrentState != State)
	{
		CurrentState = State;
		OnStateBegin.Broadcast(CurrentState);
		if(CurrentState == ECurrentState::DEAD)
			ShouldRegen.ExecuteIfBound(false);
		else
			ShouldRegen.ExecuteIfBound(true);
	}
}

void UStateManagerComponent::ResetState()
{
	SetCurrentState(ECurrentState::NOTHING);
}

void UStateManagerComponent::ResetAction()
{
	SetCurrentAction(ECurrentAction::NOTHING);
}

bool UStateManagerComponent::IsCurrentStateEqualToThis(TArray<ECurrentState> StatesToCheck)
{
	return StatesToCheck.Contains(CurrentState);
}

void UStateManagerComponent::SetCurrentAction(ECurrentAction Action)
{
	if(CurrentAction != Action)
	{
		CurrentAction = Action;
	}
}

bool UStateManagerComponent::IsCurrentActionEqualToThis(TArray<ECurrentAction> StatesToCheck)
{
	return StatesToCheck.Contains(CurrentAction);
}

void UStateManagerComponent::SetMovementType(EMovementType Type)
{
	CurrentMovementType = Type;
	if (CurrentMovementType == EMovementType::SPRINTING)
	{
		ShouldRegen.ExecuteIfBound(false);
	}
	else
	{
		ShouldRegen.ExecuteIfBound(true);
	}
	
}

void UStateManagerComponent::SetCurrentCombatState(ECurrentCombatState CombatState)
{
	CurrentCombatState = CombatState;
	if (CurrentCombatState == ECurrentCombatState::COMBAT_STATE)
	{
		OnCombatState.Broadcast(true);
	}
	else
	{
		OnCombatState.Broadcast(false);
	}
}

ECurrentCombatState UStateManagerComponent::GetCurrentCombatState()
{
	return CurrentCombatState;
}

void UStateManagerComponent::SetPotionCooldown(bool IsHPPotion)
{
	if(IsHPPotion)
	{
		IsHPPotionCooldown = true;
		PotionCooldownDelegate.BindUFunction(this, FName("PotionCooldownComplete"), IsHPPotion);
		GetWorld()->GetTimerManager().SetTimer(HPPotionTimerHandle, PotionCooldownDelegate, HPPotionCooldownTime, false);
	}
	else
	{
		IsStaminaPotionCooldown = true;
		PotionCooldownDelegate.BindUFunction(this, FName("PotionCooldownComplete"), IsHPPotion);
		GetWorld()->GetTimerManager().SetTimer(StaminaPotionTimerHandle, PotionCooldownDelegate, StaminaPotionCooldownTime, false);
	}
}

float UStateManagerComponent::GetHPPotionCurrentCooldownTime()
{
	if(IsHPPotionCooldown)
		return GetWorld()->GetTimerManager().GetTimerRemaining(HPPotionTimerHandle);
	return 0.f;
}

float UStateManagerComponent::GetStaminaPotionCurrentCooldownTime()
{
	if(IsStaminaPotionCooldown)
		return GetWorld()->GetTimerManager().GetTimerRemaining(StaminaPotionTimerHandle);
	return 0.f;
}

float UStateManagerComponent::GetDodgeCurrentCooldownTime()
{
	if(IsDodgeCooldown)
		return GetWorld()->GetTimerManager().GetTimerRemaining(DodgeTimerHandle);
	return 0.f;
}

void UStateManagerComponent::PotionCooldownComplete(bool IsHPPotion)
{
	if(IsHPPotion)
	{
		IsHPPotionCooldown = false;
	}
	else
	{
		IsStaminaPotionCooldown = false;
	}
}

void UStateManagerComponent::SetDodgeCoolDown()
{
	IsDodgeCooldown = true;
	GetWorld()->GetTimerManager().SetTimer(DodgeTimerHandle, this, &ThisClass::DodgeCooldownComplete, DodgeCooldownTime);
}

void UStateManagerComponent::DodgeCooldownComplete()
{
	IsDodgeCooldown = false;
}