#include "BaseWeapon.h"
#include "Kismet/GameplayStatics.h"

#include "MeleeCharacter.h"
#include "Type/Types.h"
#include "Component/CombatComponent.h"
#include "Component/CollisionComponent.h"


ABaseWeapon::ABaseWeapon()
{
    CollisionComponent = CreateDefaultSubobject<UCollisionComponent>(TEXT("CollisionComponent"));
    
    SetEquipmentType(EEquipmentType::WEAPON);
}


void ABaseWeapon::OnHit(FHitResult& HitResult)
{
   
    if(GetOwner() && Cast<AMeleeCharacter>(GetOwner())->GetCombatComp())
    {
        //Cast<AMeleeCharacter>(GetOwner())->GetCombatComp()->HitCauseDamage(HitResult);
        OnHitResult.ExecuteIfBound(HitResult);
    }
}

void ABaseWeapon::SimulateWeaponPhysics()
{
    GetItemMeshComp()->SetCollisionProfileName(TEXT("PhysicsActor"));
    GetItemMeshComp()->SetSimulatePhysics(true);
    GetItemMeshComp()->SetMassOverrideInKg(NAME_None, 350.f);
    GetItemMeshComp()->SetLinearDamping(-0.5f);
    GetItemMeshComp()->SetAngularDamping(5.f);
}
