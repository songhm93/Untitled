#include "BaseQuest.h"

ABaseQuest::ABaseQuest()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ABaseQuest::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseQuest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

