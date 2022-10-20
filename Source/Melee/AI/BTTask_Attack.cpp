#include "BTTask_Attack.h"
#include "AIController.h"
#include "GameFramework/Character.h"

#include "../Type/Types.h"
#include "../Component/MonstersCombatComponent.h"
#include "../Component/StateManagerComponent.h"


EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (OwnerController)
	{
		ACharacter* MonsterCharacter = Cast<ACharacter>(OwnerController->GetPawn());
        if(MonsterCharacter)
        {
            UMonstersCombatComponent* MCC = Cast<UMonstersCombatComponent>(MonsterCharacter->GetComponentByClass(UMonstersCombatComponent::StaticClass()));
            UStateManagerComponent* SMM = Cast<UStateManagerComponent>(MonsterCharacter->GetComponentByClass(UStateManagerComponent::StaticClass()));
            if(MCC && SMM && SMM->GetCurrentState() != ECurrentState::NOTHING)
            {
                MCC->LightAttack();
                return EBTNodeResult::Succeeded;
            }
        }
    }
    return EBTNodeResult::Failed;
}