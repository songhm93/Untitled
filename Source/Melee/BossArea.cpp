#include "BossArea.h"
#include "Components/SphereComponent.h"

#include "MonsterCharacter/RampageMonster.h"
#include "Interface/TargetingInterface.h"
#include "Component/StateManagerComponent.h"

ABossArea::ABossArea()
{
	PrimaryActorTick.bCanEverTick = false;

	AreaSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphereComp"));
	DefaultComp = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultComp"));
	DefaultComp->SetupAttachment(RootComponent);
	AreaSphereComp->SetupAttachment(RootComponent);
	
}

void ABossArea::Init(AEnemyCharacter* Boss)
{
	BossCharacter = Boss;
}

void ABossArea::BeginPlay()
{
	Super::BeginPlay();
	
	if(AreaSphereComp)
	{
		AreaSphereComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::AgroSphereBeginOverlap);
		AreaSphereComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::AgroSphereEndOverlap);
	}
}

void ABossArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABossArea::AgroSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	if(BossCharacter && BossCharacter->GetStateManagerComp())
	{
		if((BossCharacter->GetStateManagerComp()->GetCurrentState() == ECurrentState::DEAD)) return;
	
		if(OtherActor && OtherActor->Implements<UTargetingInterface>() && !(Cast<AEnemyCharacter>(OtherActor)))
		{
			BossCharacter->EnterCombat(OtherActor, false);
			BossCharacter->ActiveSquareArea(true);
		}
	}
}


void ABossArea::AgroSphereEndOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	if(BossCharacter && BossCharacter->GetStateManagerComp())
	{
		if(BossCharacter->GetStateManagerComp()->GetCurrentState() == ECurrentState::DEAD) return;

		if(OtherActor && OtherActor->Implements<UTargetingInterface>() && !(Cast<AEnemyCharacter>(OtherActor)))
		{
			BossCharacter->BossAgroCancel();
			BossCharacter->ActiveSquareArea(false);
		}
	}
}
