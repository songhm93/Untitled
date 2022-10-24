#include "MonsterStatsComponent.h"

#include "StateManagerComponent.h"
#include "MonstersCombatComponent.h"

UMonsterStatsComponent::UMonsterStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	
	HPRegenRate = 0.3f;
	StaminaRegenRate = 1.0f;
	bShouldRegen = true;
	SATK.Special1ATK = 0.f;
	SATK.Special2ATK = 0.f;
	SATK.Special3ATK = 0.f;
}


void UMonsterStatsComponent::BeginPlay()
{
	Super::BeginPlay();
   
	FString Path = FString(TEXT("/Game/CombatSystem/DataTable/MonsterBaseStatsTable"));
	
    InitDataTable(Path);
	if (GetOwner())
	{
		UStateManagerComponent* StateManagerComp = Cast<UStateManagerComponent>(GetOwner()->GetComponentByClass(UStateManagerComponent::StaticClass()));
		if(StateManagerComp)
		{
			StateManagerComp->ShouldRegen.BindUObject(this, &ThisClass::ShouldRegen);
			StateManagerComp->OnCombatState.AddUObject(this, &ThisClass::UpdateCombatState);
		}
			
		UMonstersCombatComponent* MonsterCombatComp = Cast<UMonstersCombatComponent>(GetOwner()->GetComponentByClass(UMonstersCombatComponent::StaticClass()));
		
		if(MonsterCombatComp)
        {
			MonsterCombatComp->GetCurrentStatValue.BindUObject(this, &ThisClass::GetCurrentStatValue);
        }
            
	}
}

void UMonsterStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!CurrentCompareMax(EStats::HP) && bShouldRegen)
		TrackingRegen(DeltaTime);
	else
		RegenTime = 0.f;
}

void UMonsterStatsComponent::TrackingRegen(float DeltaTime)
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


void UMonsterStatsComponent::InitDataTable(FString Path)
{
	UDataTable* TableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *Path));
	if(TableObject)
	{
		FBaseStatTable* Row = TableObject->FindRow<FBaseStatTable>(FName("HP"), TEXT(""));
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

void UMonsterStatsComponent::Regen()
{
	PlusCurrentStatValue(EStats::HP, HPRegenRate);
}

void UMonsterStatsComponent::UpdateCombatState(bool CombatState)
{
	if(CombatState)
		HPRegenRate = 0.3f;
	else
		HPRegenRate = 11.f;
}