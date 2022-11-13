#include "StatsComponent.h"
#include "JsonObjectConverter.h"

#include "../Type/Stats.h"
#include "StateManagerComponent.h"
#include "CombatComponent.h"
#include "../Item/Consumeable.h"

UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	HPRegenRate = 0.3f;
	StaminaRegenRate = 1.0f;
	bShouldRegen = true;

	Http = &FHttpModule::Get();
}


void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitStats();
	InitDBInfo();


	if (GetOwner())
	{
		UStateManagerComponent* StateManagerComp = Cast<UStateManagerComponent>(GetOwner()->GetComponentByClass(UStateManagerComponent::StaticClass()));
		if(StateManagerComp)
			StateManagerComp->ShouldRegen.BindUObject(this, &ThisClass::ShouldRegen);
		
		UCombatComponent* CombatComp = Cast<UCombatComponent>(GetOwner()->GetComponentByClass(UCombatComponent::StaticClass()));
		if (CombatComp)
		{
			CombatComp->OnUpdateCurrentStatValue.BindUObject(this, &ThisClass::PlusCurrentStatValue);
			CombatComp->GetCurrentStatValue.BindUObject(this, &ThisClass::GetCurrentStatValue);
		}
	}
}

void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if((!CurrentCompareMax(EStats::HP) || !CurrentCompareMax(EStats::STAMINA)) && bShouldRegen)
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
	if(CurrentStats.Contains(Stat))
	{
		if(CurrentStats[Stat] == MaxStats[Stat])
			return true;
		else
			return false;
	}

	return true;
}

void UStatsComponent::SetCurrentStatValue(EStats Stat, float Value)
{
	CurrentStats.Add(Stat, Value);
}

void UStatsComponent::SetMaxStatValue(EStats Stat, float Value)
{
	MaxStats.Add(Stat, Value);
}

float UStatsComponent::GetCurrentStatValue(EStats Stat)
{
	return CurrentStats.FindRef(Stat);
}

void UStatsComponent::InitStats()
{
	SetCurrentStatValue(EStats::HP, 0.f);
	SetCurrentStatValue(EStats::STAMINA, 0.f);
	SetCurrentStatValue(EStats::ATK, 0.f);
	SetCurrentStatValue(EStats::DEF, 0.f);

	SetMaxStatValue(EStats::HP, 0.f);
	SetMaxStatValue(EStats::STAMINA, 0.f);
	SetMaxStatValue(EStats::ATK, 0.f);
	SetMaxStatValue(EStats::DEF, 0.f);

		
}

void UStatsComponent::PlusCurrentStatValue(EStats Stat, float Value) 
{
	if(Value != 0.f)
	{
		SetCurrentStatValue(Stat, FMath::Clamp(CurrentStats[Stat] + Value, 0.f, MaxStats[Stat]));
		OnPlusCurrentStatValue.Broadcast(Stat, CurrentStats[Stat]); // 위젯으로 Broadcast
	}
}

void UStatsComponent::InitDBInfo() //DB에서 꺼내온다.
{
	FString PID = "9824"; //얘를 어떤식으로?
	
	if(Http)
	{
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
	
		Request->OnProcessRequestComplete().BindUObject(this, &UStatsComponent::OnProcessRequestComplete);
		Request->SetURL("http://localhost:8080/api/PlayerInfo/" + PID);
		Request->SetVerb("GET");
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

		Request->ProcessRequest();
	}
}

void UStatsComponent::OnProcessRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
	if(Success)
	{
        FPlayerInfo PlayerInfo = ConvertToPlayerInfo(Response->GetContentAsString());
		SetCurrentStatValue(EStats::HP, PlayerInfo.Currenthp);
		SetCurrentStatValue(EStats::STAMINA, PlayerInfo.Currentstamina);
		SetCurrentStatValue(EStats::ATK, PlayerInfo.Atk);
		SetCurrentStatValue(EStats::DEF, PlayerInfo.Def);

		SetMaxStatValue(EStats::HP, PlayerInfo.Maxhp);
		SetMaxStatValue(EStats::STAMINA, PlayerInfo.Maxstamina);
		SetMaxStatValue(EStats::ATK, PlayerInfo.Atk);
		SetMaxStatValue(EStats::DEF, PlayerInfo.Def);
	
		PlusCurrentStatValue(EStats::HP, 0.00000001f);
		PlusCurrentStatValue(EStats::STAMINA, 0.00000001f);
	}
}

FPlayerInfo UStatsComponent::ConvertToPlayerInfo(const FString& ResponseString)
{
	FPlayerInfo PlayerInfo;
    if(!ResponseString.Contains("timestamp")) //테이블에 해당하는 PID가 있을 때
    {
        FJsonObjectConverter::JsonObjectStringToUStruct(*ResponseString, &PlayerInfo, 0, 0);
    }

    return PlayerInfo;
}


float UStatsComponent::GetMaxValue(EStats Stat)
{
	if(!MaxStats.IsEmpty())
		return MaxStats[Stat];
	else
		return 0.f;
}

void UStatsComponent::ShouldRegen(bool ShouldRegen)
{
	bShouldRegen = ShouldRegen;	
}
