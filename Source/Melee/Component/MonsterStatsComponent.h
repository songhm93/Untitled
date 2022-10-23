#pragma once

#include "CoreMinimal.h"
#include "StatsComponent.h"
#include "MonsterStatsComponent.generated.h"


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

private:
	void UpdateCombatState(bool CombatState);
	UPROPERTY(EditAnywhere, Category = "Special", Meta = (AllowPrivateAccess = "true"))
	float Special1ATK;
	UPROPERTY(EditAnywhere, Category = "Special", Meta = (AllowPrivateAccess = "true"))
	float Special2ATK;
	UPROPERTY(EditAnywhere, Category = "Special", Meta = (AllowPrivateAccess = "true"))
	float Special3ATK;
};
