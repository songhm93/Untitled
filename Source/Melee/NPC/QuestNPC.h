#pragma once

#include "CoreMinimal.h"
#include "BaseNPC.h"
#include "QuestNPC.generated.h"

class UQuestWidget;
class UReceiveQuestAlreadyWidget;
class UQuestCompleteWidget;
class UWidgetComponent;


UCLASS()
class MELEE_API AQuestNPC : public ABaseNPC
{
	GENERATED_BODY()

public:
	AQuestNPC();
	virtual void Interact(AActor* Caller) override;
	
private:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UQuestWidget> QuestWidgetClass;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UReceiveQuestAlreadyWidget> ReceiveQuestAlreadyWidgetClass;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UQuestCompleteWidget> QuestCompleteWidgetClass;

	UPROPERTY()
	UQuestWidget* QuestWidget;

	UPROPERTY()
	UReceiveQuestAlreadyWidget* ReceiveQuestAlreadyWidgetWidget;

	UPROPERTY()
	UQuestCompleteWidget* QuestCompleteWidget;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TArray<int32> QIdArray;

};
