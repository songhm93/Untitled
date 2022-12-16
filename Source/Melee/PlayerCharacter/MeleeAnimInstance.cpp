#include "MeleeAnimInstance.h"

#include "BaseCharacter.h"
#include "../Component/CombatComponent.h"
#include "../Component/StateManagerComponent.h"
#include "../Type/Types.h"

void UMeleeAnimInstance::NativeInitializeAnimation()
{
    Character = Cast<ABaseCharacter>(TryGetPawnOwner());
    Direction = 0.f;
    if (Character && Character->GetCombatComp())
        Character->GetCombatComp()->OnUpdateWeaponType.BindUObject(this, &ThisClass::SetWeaponType);
    bIsDead = false;
}

void UMeleeAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    if(Character)
    {
        Speed = Character->GetVelocity().Size();
        Direction = CalculateDirection(Character->GetVelocity(), Character->GetActorRotation());
    }
}

void UMeleeAnimInstance::AnimNotify_Equip()
{
    if(Character && Character->GetCombatComp() && Character->GetCombatComp()->GetEquippedWeapon() && Character->GetStateManagerComp())
    {
        Character->GetCombatComp()->AttachWeapon();
        Character->GetStateManagerComp()->SetCurrentState(ECurrentState::NOTHING);
    }
}

void UMeleeAnimInstance::AnimNotify_UnEquip()
{
    if(Character && Character->GetCombatComp() && Character->GetCombatComp()->GetEquippedWeapon() && Character->GetStateManagerComp())
    {
        Character->GetCombatComp()->AttachWeapon();
        Character->GetStateManagerComp()->SetCurrentState(ECurrentState::NOTHING);
    }
}

void UMeleeAnimInstance::AnimNotify_ContinueAttack()
{
    if(Character && Character->GetCombatComp())
    {
        Character->GetCombatComp()->ContinueAttack();
    }
}

void UMeleeAnimInstance::AnimNotify_ResetCombat()
{
    if(Character && Character->GetStateManagerComp())
    {
        if(Character->GetStateManagerComp()->GetCurrentState() != ECurrentState::DEAD)
            Character->ResetCombat();
    }
}

void UMeleeAnimInstance::AnimNotify_DodgeComplete()
{
    if(Character && Character->GetStateManagerComp())
    {
        if(Character->GetStateManagerComp()->GetCurrentState() != ECurrentState::DEAD)
            Character->ResetCombat();

        Character->GetStateManagerComp()->SetDodgeCoolDown();
      
    }
}

void UMeleeAnimInstance::AnimNotify_Impact()
{
    OnImpact.ExecuteIfBound(0);
}

void UMeleeAnimInstance::AnimNotify_Skill3Impact()
{
    OnImpact.ExecuteIfBound(3);
}

void UMeleeAnimInstance::SetWeaponType(EWeaponType Type)
{
    WeaponType = Type;
}

void UMeleeAnimInstance::AnimNotify_UltimateComplete()
{
    OnUltimateComplete.ExecuteIfBound();
}

void UMeleeAnimInstance::AnimNotify_UltimateImpact()
{
    OnUltimateImpact.ExecuteIfBound();
}

void UMeleeAnimInstance::AnimNotify_DeathAnimComplete()
{
    bIsDead = true;
}