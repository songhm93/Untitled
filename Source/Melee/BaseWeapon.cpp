#include "BaseWeapon.h"
#include "MeleeCharacter.h"
#include "MeleeAnimInstance.h"
#include "Types.h"
#include "CombatComponent.h"

ABaseWeapon::ABaseWeapon()
{

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

void ABaseWeapon::SetAttackMontage(UAnimMontage* Montage)
{
    if(Montage)
        AttackMontage.Add(Montage);
}

void ABaseWeapon::SetDodgeMontage(UAnimMontage* Montage)
{
    if(Montage)
        DodgeMontage.Add(Montage);
}