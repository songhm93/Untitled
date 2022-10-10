#include "BaseWeapon.h"
#include "MeleeCharacter.h"
#include "MeleeAnimInstance.h"
#include "Types.h"
#include "CombatComponent.h"
#include "StateManagerComponent.h"
#include "CollisionComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseWeapon::ABaseWeapon()
{
    CollisionComponent = CreateDefaultSubobject<UCollisionComponent>(TEXT("CollisionComponent"));
    WeaponATK = 20.f;
    SetEquipmentType(EEquipmentType::WEAPON);
}


void ABaseWeapon::OnHit(FHitResult& HitResult)
{
    if(GetOwner() && Cast<AMeleeCharacter>(GetOwner())->GetCombatComp())
    {
        Cast<AMeleeCharacter>(GetOwner())->GetCombatComp()->HitCauseDamage(HitResult, this);
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
