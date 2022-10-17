#include "BaseWeapon.h"
#include "Kismet/GameplayStatics.h"

#include "../Type/Types.h"
#include "../Component/CollisionComponent.h"


ABaseWeapon::ABaseWeapon()
{
    CollisionComponent = CreateDefaultSubobject<UCollisionComponent>(TEXT("CollisionComponent"));
    
    SetEquipmentType(EEquipmentType::WEAPON);
    CurrentElement = EElements::NONE;
}


void ABaseWeapon::OnHit(FHitResult& HitResult)
{
    if(GetOwner())
        OnHitResult.ExecuteIfBound(HitResult);
}

void ABaseWeapon::SimulateWeaponPhysics()
{
    GetItemMeshComp()->SetCollisionProfileName(TEXT("PhysicsActor"));
    GetItemMeshComp()->SetSimulatePhysics(true);
    GetItemMeshComp()->SetMassOverrideInKg(NAME_None, 350.f);
    GetItemMeshComp()->SetLinearDamping(-0.5f);
    GetItemMeshComp()->SetAngularDamping(5.f);
}
