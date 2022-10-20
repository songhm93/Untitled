#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "../Type/AIBehavior.h"
#include "BTService_UpdateBehavior.generated.h"

class AEnemyCharacter;
class AAIController;
class UBlackboardComponent;

UCLASS()
class MELEE_API UBTService_UpdateBehavior : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_UpdateBehavior();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	float AttackRange;
	UPROPERTY()
	AEnemyCharacter* MonsterCharacter;
	UPROPERTY()
	AAIController* MonsterController;
	UPROPERTY()
	UBlackboardComponent* BlackBoardComp;
	EAIBehavior CurrentBehavior;

	void SetBehavior(EAIBehavior AIBehavior);
	void SetTargetCanAttackLocation(AActor* Target, float Dist);

};
