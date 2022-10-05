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

    static ConstructorHelpers::FObjectFinder<UAnimMontage> EnterCombatAM(TEXT("/Game/CombatSystem/AnimMontage/AM_HeavySword_EnterCombat"));
    if(EnterCombatAM.Succeeded())
    {
        SetEnterCombatAM(EnterCombatAM.Object);
    }
    static ConstructorHelpers::FObjectFinder<UAnimMontage> ExitCombatAM(TEXT("/Game/CombatSystem/AnimMontage/AM_HeavySword_ExitCombat"));
    if(ExitCombatAM.Succeeded())
    {
        SetExitCombatAM(ExitCombatAM.Object);
    }

     SetCombatType(ECombatType::GREAT_SWORD);
}