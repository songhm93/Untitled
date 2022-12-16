#include "BTService_UpdateBehavior.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/TargetPoint.h"

#include "../Component/StateManagerComponent.h"
#include "../MonsterCharacter/EnemyCharacter.h"
#include "../PlayerController/EnemyAIController.h"
#include "../Interface/BossInterface.h"

UBTService_UpdateBehavior::UBTService_UpdateBehavior()
{

}

void UBTService_UpdateBehavior::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	MonsterController = OwnerComp.GetAIOwner();
	
	if(MonsterController)
	{
		MonsterCharacter = Cast<AEnemyCharacter>(MonsterController->GetPawn());
		BlackBoardComp = Cast<AEnemyAIController>(MonsterController)->GetBBComp();

		if(MonsterCharacter && MonsterCharacter->GetStateManagerComp())
		{
			if((MonsterCharacter->GetStateManagerComp()->GetCurrentState() != ECurrentState::NOTHING) && (MonsterCharacter->GetStateManagerComp()->GetCurrentState() != ECurrentState::DEAD))
			{
				if(BlackBoardComp)
				{
					AActor* TargetActor = Cast<AActor>(BlackBoardComp->GetValueAsObject(TEXT("Target")));
					if (TargetActor)
					{								
						const float Dist = MonsterCharacter->GetDistanceTo(TargetActor);
						if(Dist <= MonsterCharacter->GetAttackRange())
						{
							BlackBoardComp->SetValueAsBool(TEXT("InAttackRange"), true);
							Cast<AEnemyAIController>(MonsterController)->GetBBComp()->SetValueAsBool(TEXT("OriPosReturn"), false);
							SetBasicBehavior(EAIBasicBehavior::BASIC_ATTACK);
						}
						else
						{
							SetBasicBehavior(EAIBasicBehavior::CHASE);
							BlackBoardComp->SetValueAsBool(TEXT("InAttackRange"), false);
							TArray<ATargetPoint*> PatrolPoint = MonsterCharacter->GetPatrolPoints();
							const float CenterDist = MonsterCharacter->GetDistanceTo(Cast<AActor>(PatrolPoint[0]));
							if(CenterDist > 700.f) 
							{					   
								Cast<AEnemyAIController>(MonsterController)->GetBBComp()->SetValueAsBool(TEXT("OriPosReturn"), true);
								SetBasicBehavior(EAIBasicBehavior::RETURN);
							}
						}
					}
					else
					{
						SetBasicBehavior(EAIBasicBehavior::PATROL);
					}
				}
			}
			else
			{
				SetBasicBehavior(EAIBasicBehavior::NONE);
			}
		}
	}
}

void UBTService_UpdateBehavior::SetBasicBehavior(EAIBasicBehavior AIBehavior)
{
	uint8 EnumIdx = 0;

	switch (AIBehavior)
	{
	case EAIBasicBehavior::NONE:
		EnumIdx = 0;
		break;
	case EAIBasicBehavior::BASIC_ATTACK:
		EnumIdx = 1;
		break;
	case EAIBasicBehavior::CHASE:
		EnumIdx = 2;
		break;
	case EAIBasicBehavior::PATROL:
		EnumIdx = 3;
		break;
	case EAIBasicBehavior::RETURN:
		EnumIdx = 4;
		break;
	}
	BlackBoardComp->SetValueAsEnum(BasicBehavior.SelectedKeyName, EnumIdx);
}
