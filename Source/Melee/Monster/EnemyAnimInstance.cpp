#include "EnemyAnimInstance.h"

#include "../Monster/EnemyCharacter.h"
#include "../Component/StateManagerComponent.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
    EnemyCharacter = Cast<AEnemyCharacter>(TryGetPawnOwner());
    if(EnemyCharacter)
    {
        UStateManagerComponent* StateManagerComp = EnemyCharacter->GetStateManagerComp();
        if(StateManagerComp)
        {
            StateManagerComp->OnCombatState.AddUObject(this, &ThisClass::UpdateCombatState);
        }
    }
    
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{

}

void UEnemyAnimInstance::AnimNotify_ResetCombat()
{
    if(EnemyCharacter && EnemyCharacter->GetStateManagerComp())
    {
        if(EnemyCharacter->GetStateManagerComp()->GetCurrentState() != ECurrentState::DEAD)
            EnemyCharacter->ResetCombat();
    }
}

void UEnemyAnimInstance::UpdateCombatState(bool CombatState)
{
    bCombatState = CombatState;
}