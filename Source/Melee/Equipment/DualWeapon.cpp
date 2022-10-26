#include "DualWeapon.h"

#include "../Type/CollisionPart.h"
#include "../Type/Types.h"


ADualWeapon::ADualWeapon()
{
    DualSwordStaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DualSwordStaticMeshComp"));
    DualSwordStaticMeshComp->SetupAttachment(RootComponent);
    SetWeaponType(EWeaponType::DUAL_SWORD);
    GetItemStaticMeshComp()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DualSwordStaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    
    SecondWeaponAttachSocket = TEXT("DualSword_L_Back");
    SecondWeaponHandSocket = TEXT("SecondWeaponSocket");
    SetAttachSocketName(TEXT("DualSword_R_Back"));
    SetHandSocketName(TEXT("WeaponSocket"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SecondSword(TEXT("/Game/CombatSystem/Resource/Meshes/Weapons/SM_ToughSword"));
    if(SecondSword.Succeeded())
        DualSwordStaticMeshComp->SetStaticMesh(SecondSword.Object);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sword(TEXT("/Game/CombatSystem/Resource/Meshes/Weapons/SM_ToughSword"));
    if(Sword.Succeeded())
        GetItemStaticMeshComp()->SetStaticMesh(Sword.Object);

     WeaponATK = 15.f;
}

