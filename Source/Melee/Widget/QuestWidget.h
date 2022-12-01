#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestWidget.generated.h"

class UQuestWidget;
class UTextBlock;
class UButton;
class ABaseQuest;

UCLASS()
class MELEE_API UQuestWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(ABaseQuest* QuestParam);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	ABaseQuest* Quest;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* QuestName;

	UPROPERTY(Meta = (BindWidget))
	UTextBlock* QuestDesc;

	UPROPERTY(Meta = (BindWidget))
	UButton* AcceptButton;

	UPROPERTY(Meta = (BindWidget))
	UButton* RejectButton;
};
