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
    Damage = 10.f;

    if(GetOwner())
    {
        Controller = Cast<APawn>(GetOwner())->GetController();
    }
    HitFromDirection = FVector::ZeroVector;
}

void ABaseWeapon::OnEquipped()
{
    SetIsEquipped(true);

    AMeleeCharacter* Character =  Cast<AMeleeCharacter>(GetOwner());
    
    if(Character && Character->GetCombatComp())
    {
        AttachWeapon(Character);
        Character->GetCombatComp()->SetEquippedWeapon(this);
        if(Character->GetMesh() &&  Character->GetMesh()->GetAnimInstance())
        {
            Cast<UMeleeAnimInstance>(Character->GetMesh()->GetAnimInstance())->SetWeaponType(GetWeaponType());
        }
    }
    CollisionComponent->SetCollisionMeshComponent(GetItemMesh());
    
}

void ABaseWeapon::AttachWeapon(AMeleeCharacter* Character)
{
    if(Character)
    {
        if(Character->GetCombatComp())
        {
            if(Character->GetCombatComp()->GetCombatState())
                AttachActor(GetHandSocketName());
            else
                AttachActor(GetAttachSocketName());
        }
    }
} 

void ABaseWeapon::OnHit(FHitResult& HitResult)
{
    Controller = Controller == nullptr ? Cast<APawn>(GetOwner())->GetController() : Controller;
    HitFromDirection = GetOwner()->GetActorForwardVector();
	
    if(HitResult.GetActor())
    {
        if(Cast<AMeleeCharacter>(HitResult.GetActor())->CanRecieveDamage())
        {
            UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), Damage, HitFromDirection, HitResult, Controller, this, UDamageType::StaticClass());
        }
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
