#include "PreemptiveMonster.h"
#include "Components/SphereComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "../PlayerController/EnemyAIController.h"
#include "../Component/StateManagerComponent.h"

APreemptiveMonster::APreemptiveMonster()
{

}

void APreemptiveMonster::BeginPlay()
{
    Super::BeginPlay();

    if(AgroRangeSphere)
	{
		AgroRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::AgroSphereBeginOverlap);
		AgroRangeSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::AgroSphereEndOverlap);
	}
}

void APreemptiveMonster::AgroSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	AIController = AIController == nullptr ? Cast<AEnemyAIController>(GetController()) : AIController;

	if(OtherActor && OtherActor->Implements<UTargetingInterface>() && !(Cast<AEnemyCharacter>(OtherActor)) && AIController)
	{
		if(AIController && StateManagerComp)
		{
			AIController->GetBBComp()->SetValueAsObject(TEXT("Target"), OtherActor);
			bTargetingState = true;
			Target = OtherActor;
			StateManagerComp->SetCurrentCombatState(ECurrentCombatState::COMBAT_STATE);
			AIController->GetBBComp()->SetValueAsBool(TEXT("CombatState"), true);
		}
		if(GetWorldTimerManager().IsTimerActive(AgroCancelTimerHandle))
		{
			GetWorldTimerManager().ClearTimer(AgroCancelTimerHandle);
		}
	}
}

void APreemptiveMonster::AgroSphereEndOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	AIController = AIController == nullptr ? Cast<AEnemyAIController>(GetController()) : AIController;
	
	if(OtherActor && OtherActor->Implements<UTargetingInterface>() && !(Cast<AEnemyCharacter>(OtherActor)) && AIController)
	{
		if(AIController)
		{
			GetWorldTimerManager().SetTimer(AgroCancelTimerHandle, this, &ThisClass::AgroCancel, AgroCancelTime);
		}
	}
}