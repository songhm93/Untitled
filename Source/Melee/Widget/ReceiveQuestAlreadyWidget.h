#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReceiveQuestAlreadyWidget.generated.h"

class ABaseQuest;

UCLASS()
class MELEE_API UReceiveQuestAlreadyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(ABaseQuest* QuestParam);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	ABaseQuest* Quest;
	
};
