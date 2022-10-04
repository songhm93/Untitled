#include "BaseWeapon.h"
#include "MeleeCharacter.h"

ABaseWeapon::ABaseWeapon()
{
    HandSocketName = TEXT("WeaponSocket");
    bIsAttachedToHand = false;
}

void ABaseWeapon::OnEquipped()
{
    Super::OnEquipped();

    Cast<AMeleeCharacter>(GetOwner())->SetWeapon(this);
}