#include "BaseWeapon.h"
#include "Kismet/GameplayStatics.h"

#include "../Type/Types.h"

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

}

void ABaseWeapon::Skill2()
{

}

void ABaseWeapon::Skill3()
{

}

void ABaseWeapon::SkillUltimate()
{
    
}
