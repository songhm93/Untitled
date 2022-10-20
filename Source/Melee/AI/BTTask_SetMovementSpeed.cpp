#include "BTTask_SetMovementSpeed.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

EBTNodeResult::Type UBTTask_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (OwnerController)
	{
		ACharacter* MonsterCharacter = Cast<ACharacter>(OwnerController->GetPawn());
        if(MonsterCharacter && MonsterCharacter->GetCharacterMovement())
        {
            MonsterCharacter->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
            return EBTNodeResult::Succeeded;
        }
    }
    return EBTNodeResult::Failed;
}