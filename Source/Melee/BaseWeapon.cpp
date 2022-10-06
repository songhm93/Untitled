#include "BaseWeapon.h"
#include "MeleeCharacter.h"
#include "MeleeAnimInstance.h"
#include "Types.h"
#include "CombatComponent.h"
#include "CollisionComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseWeapon::ABaseWeapon()
{
    CollisionComponent = CreateDefaultSubobject<UCollisionComponent>(TEXT("CollisionComponent"));
    Damage = 10.f;
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
            Cast<UMeleeAnimInstance>(Character->GetMesh()->GetAnimInstance())->SetCombatType(GetCombatType());
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
	//UE_LOG(LogTemp, Warning, TEXT("OnHit! %s"), *HitResult.GetActor()->GetName());
	//AActor* DamagedActor, float BaseDamage, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass
	AController* Controller = Cast<APawn>(GetOwner())->GetController();
    FVector HitFromDirection = GetOwner()->GetActorForwardVector();
	//UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, Controller, GetOwner(), UDamageType::StaticClass());

    //AActor* DamagedActor, float BaseDamage, FVector const& HitFromDirection, FHitResult const& HitInfo, AController* EventInstigator, AActor* DamageCauser, TSubclassOf<UDamageType> DamageTypeClass
    UGameplayStatics::ApplyPointDamage(HitResult.GetActor(), Damage, HitFromDirection, HitResult, Controller, this, UDamageType::StaticClass());
    
}
