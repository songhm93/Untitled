#include "QuestLogComponent.h"

#include "../Quest/BaseQuest.h"

UQuestLogComponent::UQuestLogComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	CurrentNum = 0;
}


void UQuestLogComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UQuestLogComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

void UQuestLogComponent::AddQuestToLog(ABaseQuest* Quest)
{
	ActiveQuest = Quest;
	ActiveQuest->SetQuestStatus(EQuestStatus::PROCEEDING);
}

int32 UQuestLogComponent::CheckCollectQuest(int32 MId)
{
	if(ActiveQuest)
	{
		if(ActiveQuest->GetObjective().Type == EObjectiveType::MOB_KILL)
		{
			if(ActiveQuest->GetObjective().MId == MId)
			{
				if(ActiveQuest->GetIsComplete()) return 0;
				++CurrentNum;
				
				if(ActiveQuest->GetObjective().ObjectiveNumber == CurrentNum)
				{
					ActiveQuest->SetIsComplete(true);
					UpdateObjective.Broadcast();
					CurrentNum = 0;
					return 0;
				}
				return 0;
			}
		}
		else if(ActiveQuest->GetObjective().Type == EObjectiveType::ITEM_COLLECT)
		{
			if(ActiveQuest->GetObjective().MId == MId)
			{
				return ActiveQuest->GetObjective().ItemId;
			}
		}
	}
	return 0;
}

void UQuestLogComponent::CompleteQuest()
{
	if(ActiveQuest)
	{
		CompleteQuests.AddUnique(ActiveQuest);
		ActiveQuest = nullptr;
	}
}

void UQuestLogComponent::PlusCurrentNum(int32 Amount)
{
	CurrentNum += Amount;
	if(ActiveQuest)
	{
		if(ActiveQuest->GetObjective().Type == EObjectiveType::ITEM_COLLECT)
		{
			if(ActiveQuest->GetObjective().ObjectiveNumber == CurrentNum)
			{
				ActiveQuest->SetIsComplete(true);
				UpdateObjective.Broadcast();
				CurrentNum = 0;
			}
		}
	}
}