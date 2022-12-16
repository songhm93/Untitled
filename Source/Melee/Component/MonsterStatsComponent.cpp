#include "MonsterStatsComponent.h"

#include "StateManagerComponent.h"
#include "MonstersCombatComponent.h"
#include "../MonsterCharacter/EnemyCharacter.h"

UMonsterStatsComponent::UMonsterStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	
	HPRegenRate = 0.f;
	bShouldRegen = true;
	SATK.Special1ATK = 0.f;
	SATK.Special2ATK = 0.f;
	SATK.Special3ATK = 0.f;
}


void UMonsterStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	//InitStats();
	InitInfo();
	

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

void UMonsterStatsComponent::InitStats()
{
	SetCurrentStatValue(EStats::HP, 9999.f);
	SetCurrentStatValue(EStats::ATK, 9999.f);
	SetCurrentStatValue(EStats::DEF, 9999.f);

	SetMaxStatValue(EStats::HP, 9999.f);
	SetMaxStatValue(EStats::ATK, 9999.f);
	SetMaxStatValue(EStats::DEF, 9999.f);
}


void UMonsterStatsComponent::InitInfo() 
{
	int32 MonsterId = 0;
	if(Cast<AEnemyCharacter>(GetOwner()))
	{
		MonsterId = Cast<AEnemyCharacter>(GetOwner())->GetMId();
	}
	
	FString TablePath = FString(TEXT("/Game/CombatSystem/DataTable/MonsterInfo"));
	UDataTable* MonsterInfoTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *TablePath));
	if(MonsterInfoTableObject)
	{
		FMonsterInfoTable* MonsterInfoRow = nullptr;
		MonsterInfoRow = MonsterInfoTableObject->FindRow<FMonsterInfoTable>(FName(FString::FromInt(MonsterId)), TEXT(""));
		if(MonsterInfoRow)
		{
			SetCurrentStatValue(EStats::HP, MonsterInfoRow->MaxHP);
			SetCurrentStatValue(EStats::ATK, MonsterInfoRow->ATK);
			SetCurrentStatValue(EStats::DEF, MonsterInfoRow->DEF);

			SetMaxStatValue(EStats::HP, MonsterInfoRow->MaxHP);
			SetMaxStatValue(EStats::ATK, MonsterInfoRow->ATK);
			SetMaxStatValue(EStats::DEF, MonsterInfoRow->DEF);

			
			PlusCurrentStatValue(EStats::HP, 0.0000001f);
			PlusCurrentStatValue(EStats::ATK, 0.0000001f);
			PlusCurrentStatValue(EStats::DEF, 0.0000001f);

			MonsterGives.ItemId.Add(MonsterInfoRow->ItemId);
			MonsterGives.ItemId.Add(MonsterInfoRow->ItemId2);
			MonsterGives.ItemId.Add(MonsterInfoRow->ItemId3);
			MonsterGives.Exp = MonsterInfoRow->Exp;
			MonsterGives.Gold = MonsterInfoRow->Gold;

			Cast<AEnemyCharacter>(GetOwner())->SetMonsterName(MonsterInfoRow->Name);
		}
	}
}

void UMonsterStatsComponent::Regen()
{
	PlusCurrentStatValue(EStats::HP, HPRegenRate);
}

void UMonsterStatsComponent::UpdateCombatState(bool CombatState)
{
	if(CombatState)
		HPRegenRate = GetMaxValue(EStats::HP) * 0.001f;
	else
		HPRegenRate = GetMaxValue(EStats::HP) * 0.1f;
}