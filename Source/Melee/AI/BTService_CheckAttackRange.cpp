#include "BTService_CheckAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "../Component/StateManagerComponent.h"
#include "../MonsterCharacter/EnemyCharacter.h"
#include "../PlayerController/EnemyAIController.h"


UBTService_CheckAttackRange::UBTService_CheckAttackRange()
{

}

void UBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	MonsterController = OwnerComp.GetAIOwner();
	
	if(MonsterController)
	{
		MonsterCharacter = Cast<AEnemyCharacter>(MonsterController->GetPawn());
		BlackBoardComp = Cast<AEnemyAIController>(MonsterController)->GetBBComp();

		if(MonsterCharacter && MonsterCharacter->GetStateManagerComp())
		{
			if(MonsterCharacter->GetStateManagerComp()->GetCurrentState() != ECurrentState::NOTHING)
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
						}
						else
						{
							BlackBoardComp->SetValueAsBool(TEXT("InAttackRange"), false);
						}
					}
				}
			}
		}
	}
}