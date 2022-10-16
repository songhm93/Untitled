#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"

#include "EnemyCharacter.h"


AEnemyAIController::AEnemyAIController()
{
    BBComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BBComp"));
    BTComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BTComp"));
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    
    if(InPawn)
    {
        EnemyCharacter = Cast<AEnemyCharacter>(InPawn);
        if(EnemyCharacter)
        {
            if(EnemyCharacter->GetBT() && BBComp)
            {
                BBComp->InitializeBlackboard(*(EnemyCharacter->GetBT()->BlackboardAsset));
                RunBehaviorTree(EnemyCharacter->GetBT());
                BBComp->SetValueAsBool("CanAttack", true);
            }
        }
    }
    

}