#include "ToughSword.h"
#include "Types.h"

AToughSword::AToughSword()
{
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sword(TEXT("/Game/CombatSystem/CourseFiles/Meshes/Weapons/SM_ToughSword"));
    if(Sword.Succeeded())
        GetItemStaticMesh()->SetStaticMesh(Sword.Object);
    
    GetItemStaticMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    SetAttachSocketName(TEXT("SwordHipAttachSocket"));
    SetHandSocketName(TEXT("WeaponSocket"));


    SetWeaponType(EWeaponType::LIGHT_SWORD);
}

