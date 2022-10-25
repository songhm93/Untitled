#include "BTTask_FindNextPatrolPoint.h"
#include "Engine/TargetPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

#include "../MonsterCharacter/EnemyCharacter.h"
#include "../PlayerController/EnemyAIController.h"

EBTNodeResult::Type UBTTask_FindNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    MonsterController = OwnerComp.GetAIOwner();
	if (MonsterController)
	{
		MonsterCharacter = Cast<AEnemyCharacter>(MonsterController->GetPawn());
        if(MonsterCharacter)
        {
            if(SetPathLocation()) //위치 세팅 성공
            {
                SetPathRandomLocation(); //인덱스 랜덤으로 조정
            }
            else //위치 세팅 실패.
            {
                return EBTNodeResult::Failed;
            }
            return EBTNodeResult::Succeeded;
        }
    }
    return EBTNodeResult::Failed;
}

bool UBTTask_FindNextPatrolPoint::SetPathLocation()
{
    if(MonsterCharacter && MonsterController)
    {
        int32 CurrentIdx = Cast<AEnemyAIController>(MonsterController)->GetBBComp()->GetValueAsInt(TEXT("PatrolIndex"));
        TArray<ATargetPoint*> PatrolPoint = MonsterCharacter->GetPatrolPoints();
        if(PatrolPoint.Num() > CurrentIdx)
        {
            FVector PatrolLocation = UNavigationSystemV1::ProjectPointToNavigation(this, PatrolPoint[CurrentIdx]->GetActorLocation());
            Cast<AEnemyAIController>(MonsterController)->GetBBComp()->SetValueAsVector(TEXT("PatrolLocation"), PatrolLocation);
        }
        return true;
    }
    return false;
}

void UBTTask_FindNextPatrolPoint::SetPathRandomLocation()
{
    if(MonsterCharacter && MonsterController)
    {
        TArray<ATargetPoint*> PatrolPoint = MonsterCharacter->GetPatrolPoints();
        int32 RandomValue = FMath::RandRange(1, PatrolPoint.Num() - 1);
        Cast<AEnemyAIController>(MonsterController)->GetBBComp()->SetValueAsInt(TEXT("PatrolIndex"), RandomValue);
    }
}