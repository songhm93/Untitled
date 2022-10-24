#pragma once

#include "CoreMinimal.h"
#include "StatsComponent.h"
#include "MonsterStatsComponent.generated.h"

USTRUCT()
struct FSpecialATK
{
	GENERATED_BODY()

	FSpecialATK(){}
	FSpecialATK(float S1, float S2, float S3)
	{
		Special1ATK = S1;
		Special2ATK = S2;
		Special3ATK = S3;
	}
	
	UPROPERTY(EditAnywhere, Category = "Special", Meta = (AllowPrivateAccess = "true"))
	float Special1ATK;
	UPROPERTY(EditAnywhere, Category = "Special", Meta = (AllowPrivateAccess = "true"))
	float Special2ATK;
	UPROPERTY(EditAnywhere, Category = "Special", Meta = (AllowPrivateAccess = "true"))
	float Special3ATK;

};



UCLASS()
class MELEE_API UMonsterStatsComponent : public UStatsComponent
{
	GENERATED_BODY()

public:	
	UMonsterStatsComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void InitDataTable(FString Path) override;
	virtual void TrackingRegen(float DeltaTime) override;
	virtual void Regen() override;

	UPROPERTY(EditAnywhere, Category = "Special", Meta = (AllowPrivateAccess = "true"))
	FSpecialATK SATK;

private:
	void UpdateCombatState(bool CombatState);
	
public: //get
	FORCEINLINE FSpecialATK GetSpecialATK() const { return SATK; }

public: //set
	void SetSpecialATK(FSpecialATK ATK) { SATK = ATK; }

};
