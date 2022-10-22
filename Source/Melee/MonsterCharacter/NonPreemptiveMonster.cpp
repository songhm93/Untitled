#include "NonPreemptiveMonster.h"
#include "Components/SphereComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "../PlayerController/EnemyAIController.h"
#include "../Component/StateManagerComponent.h"

ANonPreemptiveMonster::ANonPreemptiveMonster()
{

}

void ANonPreemptiveMonster::BeginPlay()
{
    Super::BeginPlay();

    if(AgroRangeSphere)
	{
		AgroRangeSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::AgroSphereEndOverlap);
	}
}

void ANonPreemptiveMonster::ReceiveDamage(
	AActor* DamagedActor, 
	float EnemyATK,
	AController* InstigatedBy, 
	FVector HitLocation, 
	UPrimitiveComponent* FHitComponent, 
	FName BoneName, 
	FVector ShotFromDirection, 
	const UDamageType* DamageType, 
	AActor* DamageCauser)
{
	if(InstigatedBy)
	{
		Super::ReceiveDamage(DamagedActor, EnemyATK, InstigatedBy, HitLocation, FHitComponent, BoneName, ShotFromDirection, DamageType, DamageCauser);
        AIController = AIController == nullptr ? Cast<AEnemyAIController>(GetController()) : AIController;

        if(DamageCauser && DamageCauser->GetOwner()->Implements<UTargetingInterface>() && !(Cast<AEnemyCharacter>(DamageCauser->GetOwner())) && AIController)
        {
            if(AIController && StateManagerComp)
            {
                AIController->GetBBComp()->SetValueAsObject(TEXT("Target"), DamageCauser->GetOwner());
                bTargetingState = true;
                Target = DamageCauser->GetOwner();
                StateManagerComp->SetCurrentCombatState(ECurrentCombatState::COMBAT_STATE);
                AIController->GetBBComp()->SetValueAsBool(TEXT("CombatState"), true);
            }
            if(GetWorldTimerManager().IsTimerActive(AgroCancelTimerHandle))
            {
                GetWorldTimerManager().ClearTimer(AgroCancelTimerHandle);
            }
        }
	}
}

void ANonPreemptiveMonster::AgroSphereEndOverlap(
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
