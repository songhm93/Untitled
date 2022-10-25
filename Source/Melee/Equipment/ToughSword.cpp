#include "ToughSword.h"

#include "../Type/Types.h"

AToughSword::AToughSword()
{
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sword(TEXT("/Game/CombatSystem/Resource/Meshes/Weapons/SM_ToughSword"));
    if(Sword.Succeeded())
        GetItemStaticMeshComp()->SetStaticMesh(Sword.Object);
    
    GetItemStaticMeshComp()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    SetAttachSocketName(TEXT("SwordHipAttachSocket"));
    SetHandSocketName(TEXT("WeaponSocket"));


    SetWeaponType(EWeaponType::LIGHT_SWORD);
    WeaponATK = 20.f;
}

