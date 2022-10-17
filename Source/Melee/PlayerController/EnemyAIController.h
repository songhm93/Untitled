#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

class AEnemyCharacter;
class UBlackboardComponent;
class UBehaviorTreeComponent;

UCLASS()
class MELEE_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();
	virtual void OnPossess(APawn* InPawn) override;

private:
	AEnemyCharacter* EnemyCharacter;
	UPROPERTY(EditAnywhere, Category = "AI Behavior", Meta = (AllowPrivateAccess = "true"))
	UBlackboardComponent* BBComp;
	UPROPERTY(EditAnywhere, Category = "AI Behavior", Meta = (AllowPrivateAccess = "true"))
	UBehaviorTreeComponent* BTComp;

public://get
	FORCEINLINE	UBlackboardComponent* GetBBComp() const { return BBComp; }
};
