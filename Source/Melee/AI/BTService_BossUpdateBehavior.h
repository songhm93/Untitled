#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "../Type/AIBehavior.h"
#include "BTService_BossUpdateBehavior.generated.h"


class AEnemyCharacter;
class AAIController;
class UBlackboardComponent;

UCLASS()
class MELEE_API UBTService_BossUpdateBehavior : public UBTService
{
	GENERATED_BODY()
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY()
	AEnemyCharacter* MonsterCharacter;

	UPROPERTY()
	AAIController* MonsterController;

	UPROPERTY()
	UBlackboardComponent* BlackBoardComp;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector BossBehavior;


	void BossCase(float Dist, AActor* Target);

	void SetBossBehavior(EBossBehavior AIBehavior);

};
