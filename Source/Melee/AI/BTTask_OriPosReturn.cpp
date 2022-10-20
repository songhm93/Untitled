#include "BTTask_OriPosReturn.h"
#include "GameFramework/Character.h"
#include "Engine/TargetPoint.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "../MonsterCharacter/EnemyCharacter.h"
#include "../PlayerController/EnemyAIController.h"

EBTNodeResult::Type UBTTask_OriPosReturn::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AEnemyAIController* MonsterController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());
	if (MonsterController)
	{
        AEnemyCharacter* MonsterCharacter = Cast<AEnemyCharacter>(MonsterController->GetPawn());
        if(MonsterCharacter)
        {
            TArray<ATargetPoint*> PatrolPoint = MonsterCharacter->GetPatrolPoints();
            Cast<AEnemyAIController>(MonsterController)->GetBBComp()->SetValueAsVector(TEXT("OriPos"), PatrolPoint[0]->GetActorLocation());
            return EBTNodeResult::Succeeded;
        }
    }
    return EBTNodeResult::Failed;
}