#include "MeleeAnimInstance.h"

#include "BaseCharacter.h"
#include "../Equipment/ToughSword.h"
#include "../Equipment/GreatSword.h"
#include "../Component/CombatComponent.h"
#include "../Component/StateManagerComponent.h"
#include "../Type/Types.h"

void UMeleeAnimInstance::NativeInitializeAnimation()
{
    Character = Cast<ABaseCharacter>(TryGetPawnOwner());
    Direction = 0.f;
    if (Character && Character->GetCombatComp())
        Character->GetCombatComp()->OnUpdateWeaponType.BindUObject(this, &ThisClass::SetWeaponType);
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

void UMeleeAnimInstance::AnimNotify_Impact()
{
    OnImpact.ExecuteIfBound();
}

void UMeleeAnimInstance::SetWeaponType(EWeaponType Type)
{
    WeaponType = Type;
}