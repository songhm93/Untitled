#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestLogComponent.generated.h"

class ABaseQuest;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateObjective);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_API UQuestLogComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuestLogComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	int32 CheckCollectQuest(int32 MId);
	UPROPERTY(BlueprintAssignable)
	FUpdateObjective UpdateObjective;

	UFUNCTION(BlueprintCallable)
	void CompleteQuest();
protected:
	virtual void BeginPlay() override;
	

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TArray<ABaseQuest*> Quests;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	ABaseQuest* ActiveQuest;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TArray<ABaseQuest*> CompleteQuests;

	UFUNCTION(BlueprintCallable)
	void AddQuestToLog(ABaseQuest* Quest);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	int32 CurrentNum;

public:
	FORCEINLINE TArray<ABaseQuest*> GetQuests() const { return Quests; }
	FORCEINLINE ABaseQuest* GetActiveQuest() const { return ActiveQuest; }

public:
	void PlusCurrentNum(int32 Amount);
};
