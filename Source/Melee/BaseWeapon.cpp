#include "BaseWeapon.h"
#include "MeleeCharacter.h"
#include "MeleeAnimInstance.h"
#include "Types.h"

ABaseWeapon::ABaseWeapon()
{

}

void ABaseWeapon::OnEquipped()
{
    
    SetIsEquipped(true);
    

    AMeleeCharacter* Character =  Cast<AMeleeCharacter>(GetOwner());
    if(Character)
    {
        if(Character->GetCombatEnabled())
            AttachActor(GetHandSocketName());
        else
            AttachActor(GetAttachSocketName());
            
        Character->SetEquippedWeapon(this);
        if(Character->GetMesh() &&  Character->GetMesh()->GetAnimInstance())
        {
            Cast<UMeleeAnimInstance>(Character->GetMesh()->GetAnimInstance())->SetCombatType(GetCombatType());
        }
        
    }
    
}