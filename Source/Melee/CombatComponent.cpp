#include "CombatComponent.h"
#include "BaseWeapon.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	EquippedWeapon = nullptr;
	AttackCount = 0;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::SetEquippedWeapon(ABaseWeapon* NewWeapon)
{
	if(EquippedWeapon)
	{
		EquippedWeapon->OnUnequipped();
		EquippedWeapon->Destroy();
	}
	EquippedWeapon = NewWeapon;
}
