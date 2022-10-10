#include "MeleeAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "MeleeCharacter.h"
#include "ToughSword.h"
#include "GreatSword.h"
#include "CombatComponent.h"
#include "StateManagerComponent.h"
#include "Types.h"

void UMeleeAnimInstance::NativeInitializeAnimation()
{
    Character = Cast<AMeleeCharacter>(TryGetPawnOwner());
}

void UMeleeAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    if(Character)
    {
        Speed = Character->GetVelocity().Size();
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