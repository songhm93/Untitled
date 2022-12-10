#include "BTTask_Attack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "../Type/Types.h"
#include "../Type/AIBehavior.h"
#include "../Component/MonstersCombatComponent.h"
#include "../Component/StateManagerComponent.h"
#include "../Interface/BossInterface.h"
#include "../PlayerController/EnemyAIController.h"
#include "../MonsterCharacter/PreemptiveMonster.h"


EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    OwnerController = OwnerComp.GetAIOwner();
	if (OwnerController)
	{
		MonsterCharacter = Cast<ACharacter>(OwnerController->GetPawn());
        UBlackboardComponent* BBC = Cast<AEnemyAIController>(OwnerController)->GetBBComp();
        AActor* TargetActor = Cast<AActor>(BBC->GetValueAsObject(TEXT("Target")));
        if(TargetActor)
        {
            if(MonsterCharacter && MonsterCharacter->Implements<UBossInterface>())
            {
                if(BossCase())
                    return EBTNodeResult::Succeeded;
                else
                    return EBTNodeResult::Failed;
            }
            else //몹인 경우
            {
                if(BasicAttack())
                    return EBTNodeResult::Succeeded;
            }
        }
        
    }
    return EBTNodeResult::Failed;
}

bool UBTTask_Attack::BasicAttack()
{
    UMonstersCombatComponent* MCC = Cast<UMonstersCombatComponent>(MonsterCharacter->GetComponentByClass(UMonstersCombatComponent::StaticClass()));
    UStateManagerComponent* SMM = Cast<UStateManagerComponent>(MonsterCharacter->GetComponentByClass(UStateManagerComponent::StaticClass()));
    if(MCC && SMM)
    {
        if(SMM->GetCurrentState() != ECurrentState::NOTHING)
            MCC->LightAttack();
        return true;
    }
    return false;
}

bool UBTTask_Attack::BossCase() //스페셜 실행
{
    if(Cast<AEnemyAIController>(OwnerController) && Cast<AEnemyAIController>(OwnerController)->GetBBComp())
    {
        uint8 BossBehavior = Cast<AEnemyAIController>(OwnerController)->GetBBComp()->GetValueAsEnum(TEXT("BossBehavior"));
        bool SpecialComplete = Cast<AEnemyAIController>(OwnerController)->GetBBComp()->GetValueAsBool(TEXT("SpecialComplete"));
        UMonstersCombatComponent* MCC = Cast<UMonstersCombatComponent>(MonsterCharacter->GetComponentByClass(UMonstersCombatComponent::StaticClass()));
        APreemptiveMonster* BossCharacter = Cast<APreemptiveMonster>(MonsterCharacter);

        if(Cast<IBossInterface>(BossCharacter))
        {
            switch (BossBehavior)
            {
            case 1:
                BasicAttack();
                break;
            case 4:
                Cast<AEnemyAIController>(OwnerController)->GetBBComp()->SetValueAsEnum(TEXT("SpecialBehavior"), 1); //계속 유지
                Cast<IBossInterface>(BossCharacter)->Special1();
                break;
            case 5:
                Cast<AEnemyAIController>(OwnerController)->GetBBComp()->SetValueAsEnum(TEXT("SpecialBehavior"), 2);
                Cast<IBossInterface>(BossCharacter)->Special2();
                break;
            case 6:
                Cast<AEnemyAIController>(OwnerController)->GetBBComp()->SetValueAsEnum(TEXT("SpecialBehavior"), 3);
                Cast<IBossInterface>(BossCharacter)->Special3();
                break;
            case 7:
                Cast<AEnemyAIController>(OwnerController)->GetBBComp()->SetValueAsEnum(TEXT("SpecialBehavior"), 4);
                Cast<IBossInterface>(BossCharacter)->Special4();
                break;
            case 8:
                Cast<AEnemyAIController>(OwnerController)->GetBBComp()->SetValueAsEnum(TEXT("SpecialBehavior"), 5);
                Cast<IBossInterface>(BossCharacter)->Special5();
                break;
            }
        }
    }
    return true;
}
