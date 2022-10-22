#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckAttackRange.generated.h"


class AEnemyCharacter;
class AAIController;
class UBlackboardComponent;

UCLASS()
class MELEE_API UBTService_CheckAttackRange : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_CheckAttackRange();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY()
	AEnemyCharacter* MonsterCharacter;
	
	UPROPERTY()
	AAIController* MonsterController;
	
	UPROPERTY()
	UBlackboardComponent* BlackBoardComp;

	
};
