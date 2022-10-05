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

    static ConstructorHelpers::FObjectFinder<UAnimMontage> EnterCombatAM(TEXT("/Game/CombatSystem/AnimMontage/AM_LightWeaponEquip"));
    if(EnterCombatAM.Succeeded())
        SetEnterCombatAM(EnterCombatAM.Object);
    
    static ConstructorHelpers::FObjectFinder<UAnimMontage> ExitCombatAM(TEXT("/Game/CombatSystem/AnimMontage/AM_LightWeaponUnequip"));
    if(ExitCombatAM.Succeeded())
        SetExitCombatAM(ExitCombatAM.Object);
    
    static ConstructorHelpers::FObjectFinder<UAnimMontage> LightAttackAM1(TEXT("/Game/CombatSystem/AnimMontage/AM_LightAttack01"));
    if(LightAttackAM1.Succeeded())
        SetAttackMontage(LightAttackAM1.Object);
    
    static ConstructorHelpers::FObjectFinder<UAnimMontage> LightAttackAM2(TEXT("/Game/CombatSystem/AnimMontage/AM_LightAttack02"));
    if(LightAttackAM2.Succeeded())
        SetAttackMontage(LightAttackAM2.Object);
    
    static ConstructorHelpers::FObjectFinder<UAnimMontage> LightAttackAM3(TEXT("/Game/CombatSystem/AnimMontage/AM_LightAttack03"));
    if(LightAttackAM3.Succeeded())
        SetAttackMontage(LightAttackAM3.Object);
    

    SetCombatType(ECombatType::LIGHT_SWORD);
    
}

