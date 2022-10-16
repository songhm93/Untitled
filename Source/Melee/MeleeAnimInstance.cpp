#include "MeleeAnimInstance.h"

#include "MeleeCharacter.h"
#include "ToughSword.h"
#include "GreatSword.h"
#include "Component/CombatComponent.h"
#include "Component/StateManagerComponent.h"
#include "Type/Types.h"

void UMeleeAnimInstance::NativeInitializeAnimation()
{
    Character = Cast<AMeleeCharacter>(TryGetPawnOwner());
    Direction = 0.f;
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
        Character->GetStateManagerComp()->SetCurrentState(ECharacterState::NOTHING);
    }
}

void UMeleeAnimInstance::AnimNotify_UnEquip()
{
    if(Character && Character->GetCombatComp() && Character->GetCombatComp()->GetEquippedWeapon() && Character->GetStateManagerComp())
    {
        Character->GetCombatComp()->AttachWeapon();
        Character->GetStateManagerComp()->SetCurrentState(ECharacterState::NOTHING);
    }
}

void UMeleeAnimInstance::AnimNotify_ContinueAttack()
{
    if(Character)
    {
        Character->ContinueAttack();
    }
}

void UMeleeAnimInstance::AnimNotify_ResetCombat()
{
    if(Character && Character->GetStateManagerComp())
    {
        if(Character->GetStateManagerComp()->GetCurrentState() != ECharacterState::DEAD)
            Character->ResetCombat();
    }
}