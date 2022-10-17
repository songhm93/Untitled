#include "StatsComponent.h"

#include "../Type/Stats.h"
#include "StateManagerComponent.h"
#include "CombatComponent.h"
#include "MonsterCombatComponent.h"



UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	
	HPRegenRate = 0.3f;
	StaminaRegenRate = 1.0f;
	bShouldRegen = true;
}


void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	FString Path = FString(TEXT("/Game/CombatSystem/DataTable/BaseStatsTable"));
	InitDataTable(Path);
	if (GetOwner())
	{
		UStateManagerComponent* StateManagerComp = Cast<UStateManagerComponent>(GetOwner()->GetComponentByClass(UStateManagerComponent::StaticClass()));
		if(StateManagerComp)
			StateManagerComp->OnSprint.BindUObject(this, &ThisClass::ShouldRegen);
		UCombatComponent* CombatComp = Cast<UCombatComponent>(GetOwner()->GetComponentByClass(UCombatComponent::StaticClass()));
		UMonsterCombatComponent* MonsterCombatComp = Cast<UMonsterCombatComponent>(GetOwner()->GetComponentByClass(UMonsterCombatComponent::StaticClass()));
		if (CombatComp)
		{
			CombatComp->OnUpdateCurrentStatValue.BindUObject(this, &ThisClass::PlusCurrentStatValue);
			CombatComp->GetCurrentStatValue.BindUObject(this, &ThisClass::GetCurrentStatValue);
		}
		if(MonsterCombatComp)
			MonsterCombatComp->GetCurrentStatValue.BindUObject(this, &ThisClass::GetCurrentStatValue);
	}
}

void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!CurrentCompareMax(EStats::HP) || !CurrentCompareMax(EStats::STAMINA) && bShouldRegen)
		TrackingRegen(DeltaTime);
	else
		RegenTime = 0.f;
}


void UStatsComponent::TrackingRegen(float DeltaTime)
{
	if(RegenTime > 0.5f)
	{
		Regen();
		RegenTime = 0.f;
	}
	else
	{
		RegenTime += DeltaTime;
	}
}

void UStatsComponent::Regen()
{
	PlusCurrentStatValue(EStats::STAMINA, StaminaRegenRate);
	PlusCurrentStatValue(EStats::HP, HPRegenRate);
}

bool UStatsComponent::CurrentCompareMax(EStats Stat) //true면 같은거
{
	if(CurrentStats.FindRef(Stat) == BaseStats[Stat].MaxValue) 
		return true;
	else
		return false;
}

void UStatsComponent::SetCurrentStatValue(EStats Stat, float Value)
{
	CurrentStats.Add(Stat, Value);
}

float UStatsComponent::GetCurrentStatValue(EStats Stat)
{
	return CurrentStats.FindRef(Stat);
}

void UStatsComponent::InitStats()
{
	for(auto Stat : BaseStats)
	{
		SetCurrentStatValue(Stat.Key, Stat.Value.BaseValue);
	}
}

void UStatsComponent::PlusCurrentStatValue(EStats Stat, float Value) 
{
	if(Value != 0.f)
	{
		SetCurrentStatValue(Stat, FMath::Clamp(CurrentStats[Stat] + Value, 0.f, BaseStats[Stat].MaxValue));
		OnPlusCurrentStatValue.Broadcast(Stat, CurrentStats[Stat]); // 위젯으로 Broadcast
	}
}

void UStatsComponent::InitDataTable(FString Path)
{
	UDataTable* TableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *Path));
	if(TableObject)
	{
		FBaseStatTable* Row = TableObject->FindRow<FBaseStatTable>(FName("HP"), TEXT(""));
		if(Row)
			BaseStats.Add(Row->Stat, FBaseStat(Row->BaseValue, Row->MaxValue));

		Row = TableObject->FindRow<FBaseStatTable>(FName("Stamina"), TEXT(""));
		if(Row)
			BaseStats.Add(Row->Stat, FBaseStat(Row->BaseValue, Row->MaxValue));

		Row = TableObject->FindRow<FBaseStatTable>(FName("Def"), TEXT(""));
		if(Row)
			BaseStats.Add(Row->Stat, FBaseStat(Row->BaseValue, Row->MaxValue));

		Row = TableObject->FindRow<FBaseStatTable>(FName("Atk"), TEXT(""));
		if(Row)
			BaseStats.Add(Row->Stat, FBaseStat(Row->BaseValue, Row->MaxValue));
		
	}
}

float UStatsComponent::GetMaxValue(EStats Stat)
{
	if(!BaseStats.IsEmpty())
		return BaseStats[Stat].MaxValue;
	else
		return 0.f;
}

void UStatsComponent::ShouldRegen(bool ShouldRegen)
{
	bShouldRegen = ShouldRegen;	
}
