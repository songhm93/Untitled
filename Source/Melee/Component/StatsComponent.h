#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Http.h"
#include "../Type/Stats.h"
#include "../Interface/DBInterface.h"
#include "StatsComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlusCurrentStatValue, EStats, Stat, float, Value);
DECLARE_DELEGATE(FOnSavePlayerData);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStatsComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnPlusCurrentStatValue OnPlusCurrentStatValue;
	FOnSavePlayerData OnSavePlayerData;
	virtual void InitStats();
	virtual void InitInfo();
protected:
	virtual void BeginPlay() override;
	virtual void TrackingRegen(float DeltaTime);
	virtual void Regen();
	bool CurrentCompareMax(EStats Stat); //current 값과 max값을 비교해서 current가 max면 채울 필요없게. 아예 트래킹 차단.
	UFUNCTION()
	void ShouldRegen(bool ShouldRegen);
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	TMap<EStats, float> CurrentStats;
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	TMap<EStats, float> MaxStats; 
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess ="true"))
	bool bShouldRegen;
	float RegenTime;
	float StaminaRegenRate;
	float HPRegenRate;
	FHttpModule* Http;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	TMap<EExpStats, float> ExpStats; //레벨, 경험치.
	
private:
	void OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
	FPlayerInfoDB ConvertToPlayerInfo(const FString& ResponseString);
	void OnSkillUpComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);
	
public: //get	

public: //set
	FORCEINLINE void SetShouldRegen(bool Boolean){ bShouldRegen = Boolean; }
	
public:
	void SetCurrentStatValue(EStats Stat, float Value);
	void SetMaxStatValue(EStats Stat, float Value);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentStatValue(EStats Stat);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetMaxValue(EStats Stat);
	UFUNCTION()
	void PlusCurrentStatValue(EStats Stat, float Value);
	void SetExpStatValue(EExpStats ExpStat, float Value);
	void CalcMaxExp();
	float GetExpStatValue(EExpStats Stat);
	void PlusExp(float Value);
	UFUNCTION(BlueprintCallable)
	void UpdateSkillLevel(int32 SkillNum, int32 WeaponId);
	void LevelUp(int32 CurrentExp, int32 MaxExp, int32 Value);
	void DevLevelUp(); //Test용도
};
