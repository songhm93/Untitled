#include "NonPreemptiveMonster.h"
#include "Components/SphereComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"

#include "../PlayerController/EnemyAIController.h"
#include "../Component/StateManagerComponent.h"
#include "../Widget/EnemyHPBarWidget.h"
#include "../Component/MonsterStatsComponent.h"

ANonPreemptiveMonster::ANonPreemptiveMonster()
{
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
    HPBarWidget->SetupAttachment(GetMesh());

	
	HideHPBarTime = 3.f;
}

void ANonPreemptiveMonster::BeginPlay()
{
    Super::BeginPlay();

	if(StateManagerComp)
	{
        StateManagerComp->OnCombatState.AddUObject(this, &ThisClass::HPBarOnOff);
    }

    if(AgroRangeSphere)
	{
		AgroRangeSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::AgroSphereEndOverlap);
	}

	
	if(HPBarWidget)
	{
		UUserWidget* EnemyHPBarWidget = CreateWidget<UEnemyHPBarWidget>(GetWorld(), HPBarWidget->GetWidgetClass());
		if(EnemyHPBarWidget)
		{
			HPBarWidget->SetWidget(EnemyHPBarWidget);
			HPBarWidget->SetVisibility(false);
			Cast<UEnemyHPBarWidget>(EnemyHPBarWidget)->Init(MonsterStatComp);
		}
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


void ANonPreemptiveMonster::OnTargeted(bool IsTargeted)
{
	Super::OnTargeted(IsTargeted);

	if(HPBarWidget)
	{
		if(IsTargeted)
			HPBarWidget->SetVisibility(IsTargeted);
	}

}

void ANonPreemptiveMonster::HideHPBar()
{
	if(HPBarWidget)
	{
		HPBarWidget->SetVisibility(false);
	}
}

void ANonPreemptiveMonster::HPBarOnOff(bool Show)
{
	if(HPBarWidget)
	{
		if(Show)
		{
			HPBarWidget->SetVisibility(Show);
		}
		else
		{
			GetWorldTimerManager().SetTimer(HideHPBarTimerHandle, this, &ThisClass::HideHPBar, HideHPBarTime);
		}
	}
}

void ANonPreemptiveMonster::Dead()
{
	Super::Dead();

	if(HPBarWidget)
		HPBarWidget->SetVisibility(false);
}