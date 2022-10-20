#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindNextPatrolPoint.generated.h"

class AEnemyCharacter;
class AAIController;

UCLASS()
class MELEE_API UBTTask_FindNextPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	bool SetPathLocation();
	void SetPathRandomLocation();

	int32 PatrolIndex;
	FVector TargetLocation;
	UPROPERTY()
	AEnemyCharacter* MonsterCharacter;
	UPROPERTY()
	AAIController* MonsterController;
};
