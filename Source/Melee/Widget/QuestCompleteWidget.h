#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestCompleteWidget.generated.h"


class ABaseQuest;

UCLASS()
class MELEE_API UQuestCompleteWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(ABaseQuest* QuestParam);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	ABaseQuest* Quest;

	
};
