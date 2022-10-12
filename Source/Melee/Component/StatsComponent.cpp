#include "StatsComponent.h"
#include "../Type/Stats.h"



UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	
	HPRegenRate = 0.0f;
	StaminaRegenRate = 1.0f;
}


void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	FString Path = FString(TEXT("/Game/CombatSystem/DataTable/BaseStatsTable"));
	InitDataTable(Path);
	
}

void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!CurrentCompareMax(EStats::HP) || !CurrentCompareMax(EStats::STAMINA))
		TrackingRegen(DeltaTime);
	else
		RegenTime = 0.f;
}

//체력 update, 스태미너 update. 자동 리젠. 나중에 힐밴같은게 추가되면 변수추가 해야함
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

void UStatsComponent::SetBaseStatValue(EStats Stat, float Value) //이건 뭐
{
	BaseStats.Add(Stat, FBaseStat(Value, BaseStats[Stat].MaxValue));
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
		//SetCurrentStatValue(Stat.Key, 0); //증가 디버깅용
	}
}

void UStatsComponent::PlusCurrentStatValue(EStats Stat, float Value) 
{
	if(Value != 0.f)
	{
		SetCurrentStatValue(Stat, FMath::Clamp(CurrentStats[Stat] + Value, 0.f, BaseStats[Stat].MaxValue));
		OnPlusCurrentStatValue.Broadcast(Stat, CurrentStats[Stat]); //블루프린트 위젯으로 broadcast
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

