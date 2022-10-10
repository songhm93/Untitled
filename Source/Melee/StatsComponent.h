#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Stats.h"
#include "Engine/DataTable.h"
#include "StatsComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlusCurrentStatValue, EStats, Stat, float, Value);

USTRUCT(BlueprintType)
struct FBaseStat
{
	GENERATED_BODY()
	FBaseStat(){}
	FBaseStat(float Base, float Max) 
	{
		BaseValue = Base;
		MaxValue = Max;
	}

	float BaseValue;
	float MaxValue;
};

USTRUCT(BlueprintType)
struct FBaseStatTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStats Stat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxValue;
	
}; /** 데이터 테이블 추가시 에디터 끄고 컴파일 할 것 */


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStatsComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnPlusCurrentStatValue OnPlusCurrentStatValue;
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TMap<EStats, float> CurrentStats;

	UPROPERTY(VisibleAnywhere)
	TMap<EStats, FBaseStat> BaseStats; //데이터 테이블로부터 받음

	void InitDataTable(FString Path);
	void TrackingRegen(float DeltaTime);
	
	void Regen();
	float RegenTime;
	float StaminaRegenRate;
	float HPRegenRate;

public: //get	
	FORCEINLINE TMap<EStats, FBaseStat> GetBaseStats() const { return BaseStats; }
	
public: //set

public:
	void InitStats();
	void SetCurrentStatValue(EStats Stat, float Value);
	void SetBaseStatValue(EStats Stat, float Value);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentStatValue(EStats Stat);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxValue(EStats Stat);
	void PlusCurrentStatValue(EStats Stat, float Value);
	bool CurrentCompareMax(EStats Stat); //current 값과 max값을 비교해서 current가 max면 채울 필요없게. 아예 트래킹 차단.
		
};
