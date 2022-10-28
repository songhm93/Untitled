#include "BaseWeapon.h"
#include "Kismet/GameplayStatics.h"

#include "../Type/Types.h"
#include "../Component/StateManagerComponent.h"

ABaseWeapon::ABaseWeapon()
{
    SetEquipmentType(EEquipmentType::WEAPON);
    CurrentElement = EElements::NONE;

}

void ABaseWeapon::SimulateWeaponPhysics()
{
    GetItemMeshComp()->SetCollisionProfileName(TEXT("PhysicsActor"));
    GetItemMeshComp()->SetSimulatePhysics(true);
    GetItemMeshComp()->SetMassOverrideInKg(NAME_None, 350.f);
    GetItemMeshComp()->SetLinearDamping(-0.5f);
    GetItemMeshComp()->SetAngularDamping(5.f);
}

void ABaseWeapon::Skill1()
{
    UStateManagerComponent* StateManager = Cast<UStateManagerComponent>(GetOwner()->GetComponentByClass(UStateManagerComponent::StaticClass()));
    if(StateManager)
    {
        StateManager->SetCurrentState(ECurrentState::ATTACKING);
    }
}

void ABaseWeapon::Skill2()
{
    UStateManagerComponent* StateManager = Cast<UStateManagerComponent>(GetOwner()->GetComponentByClass(UStateManagerComponent::StaticClass()));
    if(StateManager)
    {
        StateManager->SetCurrentState(ECurrentState::ATTACKING);
    }
}

void ABaseWeapon::Skill3()
{
    UStateManagerComponent* StateManager = Cast<UStateManagerComponent>(GetOwner()->GetComponentByClass(UStateManagerComponent::StaticClass()));
    if(StateManager)
    {
        StateManager->SetCurrentState(ECurrentState::ATTACKING);
    }
}

void ABaseWeapon::SkillUltimate()
{
    UStateManagerComponent* StateManager = Cast<UStateManagerComponent>(GetOwner()->GetComponentByClass(UStateManagerComponent::StaticClass()));
    if(StateManager)
    {
        StateManager->SetCurrentState(ECurrentState::ATTACKING);
    }
}
