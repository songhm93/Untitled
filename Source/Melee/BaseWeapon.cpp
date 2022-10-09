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
    ATK = 20.f;
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
    GetItemMesh()->SetCollisionProfileName(TEXT("PhysicsActor"));
    GetItemMesh()->SetSimulatePhysics(true);
    GetItemMesh()->SetMassOverrideInKg(NAME_None, 350.f);
    GetItemMesh()->SetLinearDamping(-0.5f);
    GetItemMesh()->SetAngularDamping(5.f);
}
