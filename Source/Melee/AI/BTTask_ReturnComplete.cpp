#include "BTTask_ReturnComplete.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "../MonsterCharacter/EnemyCharacter.h"
#include "../PlayerController/EnemyAIController.h"

EBTNodeResult::Type UBTTask_ReturnComplete::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemyAIController* MonsterController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (MonsterController)
	{
        AEnemyCharacter* MonsterCharacter = Cast<AEnemyCharacter>(MonsterController->GetPawn());
        if(MonsterCharacter)
        {
            Cast<AEnemyAIController>(MonsterController)->GetBBComp()->SetValueAsBool(TEXT("OriPosReturn"), false);
            MonsterCharacter->AgroCancel();

            return EBTNodeResult::Succeeded;
        }
    }
    return EBTNodeResult::Failed;
}
