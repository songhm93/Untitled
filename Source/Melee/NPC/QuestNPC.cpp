#include "QuestNPC.h"
#include "Components/WidgetComponent.h"

#include "../Quest/BaseQuest.h"
#include "../Widget/QuestWidget.h"
#include "../Widget/ReceiveQuestAlreadyWidget.h"
#include "../Widget/QuestCompleteWidget.h"
#include "../PlayerCharacter/BaseCharacter.h"
#include "../Component/QuestLogComponent.h"



AQuestNPC::AQuestNPC()
{

}

void AQuestNPC::Interact(AActor* Caller)
{
    Super::Interact(Caller);

    ABaseCharacter* Player = Cast<ABaseCharacter>(Caller);

    TArray<AActor*, FDefaultAllocator> OutActors;
    GetAttachedActors(OutActors);
    OutActors.Sort();
    for(auto Actors : OutActors)
    {
        if(Cast<ABaseQuest>(Actors))
        {
            if(Cast<ABaseQuest>(Actors)->GetIsComplete() && !(Cast<ABaseQuest>(Actors)->GetQuestStatus() == EQuestStatus::COMPLETE))
            {
                QuestCompleteWidget = CreateWidget<UQuestCompleteWidget>(GetWorld(), QuestCompleteWidgetClass);
                if(QuestCompleteWidget)
                {
                    QuestCompleteWidget->Init(Cast<ABaseQuest>(Actors));
                    QuestCompleteWidget->AddToViewport();
                    return;
                }
                
            }
            else if(Player && Player->GetQuestLogComp() && Player->GetQuestLogComp()->GetActiveQuest() == Cast<ABaseQuest>(Actors))
            {
                ReceiveQuestAlreadyWidgetWidget = CreateWidget<UReceiveQuestAlreadyWidget>(GetWorld(), ReceiveQuestAlreadyWidgetClass);
                if(ReceiveQuestAlreadyWidgetWidget)
                {
                    ReceiveQuestAlreadyWidgetWidget->Init(Cast<ABaseQuest>(Actors));
                    ReceiveQuestAlreadyWidgetWidget->AddToViewport();
                    return;
                }
            }
            else if(!Cast<ABaseQuest>(Actors)->GetIsComplete())
            {
                QuestWidget = CreateWidget<UQuestWidget>(GetWorld(), QuestWidgetClass);
                if(QuestWidget)
                {
                    QuestWidget->Init(Cast<ABaseQuest>(Actors));
                    QuestWidget->AddToViewport();
                    return;
                }
            }
        }
    }
    VisibleMent(true); //퀘스트가 없을 때
}