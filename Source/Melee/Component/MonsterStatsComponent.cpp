#include "MonsterStatsComponent.h"
#include "JsonObjectConverter.h"

#include "StateManagerComponent.h"
#include "MonstersCombatComponent.h"
#include "../MonsterCharacter/EnemyCharacter.h"

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

	InitStats();
	InitDBInfo();

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


void UMonsterStatsComponent::InitDBInfo() //DB에서 꺼내온다.
{
	int32 MonsterId = 0;
	if(Cast<AEnemyCharacter>(GetOwner()))
	{
		MonsterId = Cast<AEnemyCharacter>(GetOwner())->GetMId();
	}
	FString MID = FString::FormatAsNumber(MonsterId);
	
	if(Http)
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

		Request->OnProcessRequestComplete().BindUObject(this, &UMonsterStatsComponent::OnProcessRequestComplete);
		Request->SetURL("http://localhost:8080/api/MonsterInfo/" + MID);
		Request->SetVerb("GET");
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

		Request->ProcessRequest();
	}

}

void UMonsterStatsComponent::OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
	if(Success)
	{
        FMonsterInfo MonsterInfo = ConvertToMonsterInfo(Response->GetContentAsString());

		SetCurrentStatValue(EStats::HP, MonsterInfo.Maxhp);
		SetCurrentStatValue(EStats::ATK, MonsterInfo.Atk);
		SetCurrentStatValue(EStats::DEF, MonsterInfo.Def);

		SetMaxStatValue(EStats::HP, MonsterInfo.Maxhp);
		SetMaxStatValue(EStats::ATK, MonsterInfo.Atk);
		SetMaxStatValue(EStats::DEF, MonsterInfo.Def);
        
	}
}

FMonsterInfo UMonsterStatsComponent::ConvertToMonsterInfo(const FString& ResponseString)
{
	FMonsterInfo MonsterInfo;
    if(!ResponseString.Contains("timestamp")) //테이블에 해당하는 MID가 있을 때
    {
        FJsonObjectConverter::JsonObjectStringToUStruct(*ResponseString, &MonsterInfo, 0, 0);
    }

    return MonsterInfo;
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