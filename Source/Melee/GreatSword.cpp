#include "GreatSword.h"
#include "Types.h"

AGreatSword::AGreatSword()
{
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sword(TEXT("/Game/CombatSystem/CourseFiles/Meshes/Weapons/SM_GreatSword"));
    if(Sword.Succeeded())
    {
        GetItemStaticMesh()->SetStaticMesh(Sword.Object);
    }
    GetItemStaticMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    SetAttachSocketName(TEXT("GreatSwordBackSocket"));
    SetHandSocketName(TEXT("GreatSwordHandSocket"));


     SetWeaponType(EWeaponType::GREAT_SWORD);
}