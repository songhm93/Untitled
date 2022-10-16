#include "DualWeapon.h"

#include "Type/CollisionPart.h"
#include "Type/Types.h"
#include "Component/CollisionComponent.h"


ADualWeapon::ADualWeapon()
{
    DualSwordStaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DualSwordStaticMeshComp"));
    DualSwordStaticMeshComp->SetupAttachment(RootComponent);
    SecondWeaponCollisionComp = CreateDefaultSubobject<UCollisionComponent>(TEXT("SecondWeaponCollisionComp"));
    RightFootCollisionComp = CreateDefaultSubobject<UCollisionComponent>(TEXT("RightFootCollisionComp"));
    SetWeaponType(EWeaponType::DUAL_SWORD);
    GetItemStaticMeshComp()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DualSwordStaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    RightFootCollisionComp->SetStartSocketName(FName("foot_r_Socket"));
    RightFootCollisionComp->SetEndSocketName(FName("foot_r_Socket"));
    
    SecondWeaponAttachSocket = TEXT("DualSword_L_Back");
    SecondWeaponHandSocket = TEXT("SecondWeaponSocket");
    SetAttachSocketName(TEXT("DualSword_R_Back"));
    SetHandSocketName(TEXT("WeaponSocket"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SecondSword(TEXT("/Game/CombatSystem/CourseFiles/Meshes/Weapons/SM_ToughSword"));
    if(SecondSword.Succeeded())
        DualSwordStaticMeshComp->SetStaticMesh(SecondSword.Object);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sword(TEXT("/Game/CombatSystem/CourseFiles/Meshes/Weapons/SM_ToughSword"));
    if(Sword.Succeeded())
        GetItemStaticMeshComp()->SetStaticMesh(Sword.Object);

     WeaponATK = 15.f;
}

void ADualWeapon::ActivateCollision(ECollisionPart CollisionPart)
{
    if(GetCollisionComp())
    {
        switch (CollisionPart)
        {
        case ECollisionPart::MAIN_WEAPON:
            GetCollisionComp()->EnableCollision();
            break;
        case ECollisionPart::SECOND_WEAPON:
            SecondWeaponCollisionComp->EnableCollision();
            break;
        case ECollisionPart::RIGHT_FOOT:
            RightFootCollisionComp->EnableCollision();
        }
    }  
}

void ADualWeapon::DeactivateCollision(ECollisionPart CollisionPart)
{
    if(GetCollisionComp())
    {
        switch (CollisionPart)
        {
        case ECollisionPart::MAIN_WEAPON:
            GetCollisionComp()->DisableCollision();
            break;
        case ECollisionPart::SECOND_WEAPON:
            SecondWeaponCollisionComp->DisableCollision();
            break;
        case ECollisionPart::RIGHT_FOOT:
            RightFootCollisionComp->DisableCollision();
            break;
        }
    }
     
}