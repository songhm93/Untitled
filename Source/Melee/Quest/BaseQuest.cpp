#include "BaseQuest.h"

ABaseQuest::ABaseQuest()
{
	PrimaryActorTick.bCanEverTick = false;
	QuestStatus = EQuestStatus::AVAILABLE;
}

void ABaseQuest::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseQuest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
