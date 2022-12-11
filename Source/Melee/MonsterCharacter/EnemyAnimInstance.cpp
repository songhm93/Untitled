#include "EnemyAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "../MonsterCharacter/EnemyCharacter.h"
#include "../MonsterCharacter/RampageMonster.h"
#include "../Component/StateManagerComponent.h"
#include "../PlayerController/EnemyAIController.h"


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
    SpecialReadyTime = 0.f;
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

void UEnemyAnimInstance::AnimNotify_SpecialComplete()
{
    if(EnemyCharacter)
    {
        AEnemyAIController* EnemyController = Cast<AEnemyAIController>(EnemyCharacter->GetController());
        if(EnemyController && EnemyController->GetBBComp())
        {
            SpecialReadyTime = FMath::RandRange(4.f, 5.f);
            EnemyController->GetBBComp()->SetValueAsBool(TEXT("SpecialComplete"), true);
            EnemyController->GetBBComp()->SetValueAsBool(TEXT("SpecialReady"), false);
            GetWorld()->GetTimerManager().SetTimer(SpecialReadyTimerHandle, this, &ThisClass::UpdateSpecialReady, SpecialReadyTime);
        }
    }
    
}

void UEnemyAnimInstance::UpdateSpecialReady()
{
    if(EnemyCharacter)
    {
        AEnemyAIController* EnemyController = Cast<AEnemyAIController>(EnemyCharacter->GetController());
        if(EnemyController && EnemyController->GetBBComp())
        {
            EnemyController->GetBBComp()->SetValueAsBool(TEXT("SpecialReady"), true);
        }
    }
}

void UEnemyAnimInstance::AnimNotify_DeattachRock()
{
    
	OnDeattachRock.ExecuteIfBound();
    
}

void UEnemyAnimInstance::AnimNotify_Impact()
{
    OnEnemyImpact.ExecuteIfBound();
}

void UEnemyAnimInstance::AnimNotify_DefaultCameraShake()
{
    if(EnemyCharacter)
    {
        EnemyCharacter->PlayCameraShake(true);
    }
}