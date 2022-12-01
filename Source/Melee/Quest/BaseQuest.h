#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "BaseQuest.generated.h"



UENUM(BlueprintType)
enum class EQuestType : uint8
{
	NONE UMETA(DisplayName="NONE"),
	
	MOB_KILL UMETA(DisplayName="MOB_KILL"),
	ITEM_COLLECT UMETA(DisplayName="ITEM_COLLECT"),
	MOVE_LOCATION UMETA(DisplayName="MOVE_LOCATION"),
	INTERACT UMETA(DisplayName="INTERACT"),

	MAX UMETA(DisplayName="MAX")
};

UENUM(BlueprintType)
enum class EObjectiveType : uint8
{
	NONE UMETA(DisplayName="NONE"),
	
	MOB_KILL UMETA(DisplayName="MOB_KILL"),
	ITEM_COLLECT UMETA(DisplayName="ITEM_COLLECT"),
	MOVE_LOCATION UMETA(DisplayName="MOVE_LOCATION"),
	INTERACT UMETA(DisplayName="INTERACT"),

	MAX UMETA(DisplayName="MAX")
};

USTRUCT(BlueprintType)
struct FObjectiveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Desc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EObjectiveType Type;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsComplete = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ObjectiveNumber = 0;

	
};

UENUM(BlueprintType)
enum class EQuestStatus : uint8
{
	NONE UMETA(DisplayName="NONE"),
	
	UNAVAILABLE UMETA(DisplayName="UNAVAILABLE"),
	AVAILABLE UMETA(DisplayName="AVAILABLE"),
	PROCEEDING UMETA(DisplayName="PROCEEDING"),
	READY UMETA(DisplayName="READY"),
	COMPLETE UMETA(DisplayName="COMPLETE"),

	MAX UMETA(DisplayName="MAX")
};

USTRUCT(BlueprintType)
struct FMobKillQuestInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 QId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MonsterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 KillAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MId;
};

USTRUCT(BlueprintType)
struct FItemCollectQuestInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 QId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemId;
	
};

USTRUCT(BlueprintType)
struct FQuestInfoTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 QId; //PK

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NPCId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EQuestType QuestType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PrecedingQuest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMobKillQuestInfo MobKillQuestInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FItemCollectQuestInfo ItemCollectQuestInfo;
};



USTRUCT(BlueprintType)
struct FQuestInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 QId;

	UPROPERTY(EditAnywhere)
	int32 NPCId;

	EQuestType QuestType;

	EQuestStatus QuestStatus;
};

USTRUCT(BlueprintType)
struct FRewardInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount;

};


UCLASS(Abstract)
class MELEE_API ABaseQuest : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseQuest();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	

private:
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	FQuestInfo QuestInfo;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	FText QuestName;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	FText QuestDesc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	FObjectiveData Objective;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	bool IsComplete;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	int32 RewardGold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	FRewardInfo Reward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	int32 RewardExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	EQuestStatus QuestStatus;

public: //get
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FText GetQuestName() const { return QuestName;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FText GetQuestDesc() const { return QuestDesc;}

	FORCEINLINE FObjectiveData GetObjective() const { return Objective;}
	FORCEINLINE bool GetIsComplete() const { return IsComplete;}
	FORCEINLINE EQuestStatus GetQuestStatus() const { return QuestStatus;}

public:
	FORCEINLINE void SetIsComplete(bool Boolean) { IsComplete = Boolean;}
	FORCEINLINE void SetQuestStatus(EQuestStatus Status) { QuestStatus = Status; }
};
