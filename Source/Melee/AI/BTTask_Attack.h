#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack.generated.h"

class AAIController;
class ACharacter;

UCLASS()
class MELEE_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	bool BasicAttack();
	
	bool BossCase();

	UPROPERTY()
	AAIController* OwnerController;

	UPROPERTY()
	ACharacter* MonsterCharacter;
};
