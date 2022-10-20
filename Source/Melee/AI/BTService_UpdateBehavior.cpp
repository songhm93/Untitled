#include "BTService_UpdateBehavior.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/TargetPoint.h"

#include "../Component/StateManagerComponent.h"
#include "../MonsterCharacter/EnemyCharacter.h"
#include "../PlayerController/EnemyAIController.h"


UBTService_UpdateBehavior::UBTService_UpdateBehavior()
{
	AttackRange = 150.f;
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
			if(MonsterCharacter->GetStateManagerComp()->GetCurrentState() != ECurrentState::NOTHING)
			{
				if(BlackBoardComp)
				{
					AActor* TargetActor = Cast<AActor>(BlackBoardComp->GetValueAsObject(TEXT("Target")));
					if (TargetActor)
					{
						const float Dist = MonsterCharacter->GetDistanceTo(TargetActor);
						SetBehavior(EAIBehavior::CHASE);
						if(Dist <= AttackRange)
						{
							BlackBoardComp->SetValueAsBool(TEXT("InAttackRange"), true);
							Cast<AEnemyAIController>(MonsterController)->GetBBComp()->SetValueAsBool(TEXT("OriPosReturn"), false);
							SetBehavior(EAIBehavior::ATTACK);
						}
						else
						{
							BlackBoardComp->SetValueAsBool(TEXT("InAttackRange"), false);
							//SetTargetCanAttackLocation(TargetActor , Dist); //타겟을 때릴 수 있는 위치까지만 이동
							TArray<ATargetPoint*> PatrolPoint = MonsterCharacter->GetPatrolPoints();
							const float CenterDist = MonsterCharacter->GetDistanceTo(Cast<AActor>(PatrolPoint[0]));
							if(CenterDist > 540.f)
							{
								Cast<AEnemyAIController>(MonsterController)->GetBBComp()->SetValueAsBool(TEXT("OriPosReturn"), true);
								SetBehavior(EAIBehavior::RETURN);
							}
						}
					}
					else
					{
						SetBehavior(EAIBehavior::PATROL);
					}
				}
			}
			else
			{
				SetBehavior(EAIBehavior::NONE);
			}
		}
	}
	
}

void UBTService_UpdateBehavior::SetBehavior(EAIBehavior AIBehavior)
{
	uint8 EnumIdx = 0;
	CurrentBehavior = AIBehavior;
	switch (AIBehavior)
	{
	case EAIBehavior::NONE:
		EnumIdx = 0;
		break;
	case EAIBehavior::ATTACK:
		EnumIdx = 1;
		break;
	case EAIBehavior::CHASE:
		EnumIdx = 2;
		break;
	case EAIBehavior::PATROL:
		EnumIdx = 3;
		break;
	}
	BlackBoardComp->SetValueAsEnum(TEXT("AIBehavior"), EnumIdx);
}

void UBTService_UpdateBehavior::SetTargetCanAttackLocation(AActor* Target, float Dist)
{
	if(MonsterCharacter && Target && BlackBoardComp)
	{
		FVector DirectionTowardTarget = (Target->GetActorLocation() - MonsterCharacter->GetActorLocation()).GetSafeNormal();
		float DistToGo = Dist - (AttackRange / 3);
		DirectionTowardTarget *= DistToGo;
		const FVector ResultVector = MonsterCharacter->GetActorLocation() + DirectionTowardTarget;

		BlackBoardComp->SetValueAsVector(TEXT("TargetCanAttackLocation"), ResultVector);
		

	}
}