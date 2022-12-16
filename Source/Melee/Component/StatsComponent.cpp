#include "StatsComponent.h"
#include "JsonObjectConverter.h"

#include "StateManagerComponent.h"
#include "CombatComponent.h"
#include "../Type/Stats.h"
#include "../Item/Consumeable.h"
#include "../Equipment/BaseWeapon.h"

UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	HPRegenRate = 8.f;
	bShouldRegen = true;

	Http = &FHttpModule::Get();
}


void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	
	//InitStats();
	InitInfo();


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
	
	if(!CurrentCompareMax(EStats::HP) && bShouldRegen)
		TrackingRegen(DeltaTime);
	else
		RegenTime = 0.f;
}


void UStatsComponent::TrackingRegen(float DeltaTime)
{
	if(RegenTime > 1.f)
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
	HPRegenRate = GetMaxValue(EStats::HP) * 0.001f;
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
	SetCurrentStatValue(EStats::ATK, 0.f);
	SetCurrentStatValue(EStats::DEF, 0.f);

	SetMaxStatValue(EStats::HP, 0.f);
	SetMaxStatValue(EStats::ATK, 0.f);
	SetMaxStatValue(EStats::DEF, 0.f);

		
}

void UStatsComponent::PlusCurrentStatValue(EStats Stat, float Value) 
{
	if(Value != 0.f)
	{
		if(Stat == EStats::ATK || Stat == EStats::DEF)
		{
			SetCurrentStatValue(Stat, CurrentStats[Stat] + Value);
		}
		else
		{
			SetCurrentStatValue(Stat, FMath::Clamp(CurrentStats[Stat] + Value, 0.f, MaxStats[Stat]));
		}
		
		OnPlusCurrentStatValue.Broadcast(Stat, CurrentStats[Stat]); // 위젯으로 Broadcast
	}
}

void UStatsComponent::InitInfo() //DB에서 꺼내온다.
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
        FPlayerInfoDB PlayerInfo = ConvertToPlayerInfo(Response->GetContentAsString());
		
		SetCurrentStatValue(EStats::HP, PlayerInfo.Currenthp);
		SetCurrentStatValue(EStats::ATK, PlayerInfo.Atk);
		SetCurrentStatValue(EStats::DEF, PlayerInfo.Def);

		SetMaxStatValue(EStats::HP, PlayerInfo.Maxhp);
	
		PlusCurrentStatValue(EStats::HP, 0.00000001f);
	}
}

FPlayerInfoDB UStatsComponent::ConvertToPlayerInfo(const FString& ResponseString)
{
	FPlayerInfoDB PlayerInfo;
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

void UStatsComponent::SetExpStatValue(EExpStats ExpStat, float Value)
{
	ExpStats.Add(ExpStat, Value);
}

void UStatsComponent::CalcMaxExp()
{
	int32 CurrentLevel = ExpStats[EExpStats::LEVEL];
	int32 NextLevel = CurrentLevel + 1;
	int32 MaxExp = (NextLevel - 1) * (NextLevel - 1) * (NextLevel * NextLevel - (13 * NextLevel) + 82);

	
	ExpStats.Add(EExpStats::MAX_EXP, MaxExp);
}

float UStatsComponent::GetExpStatValue(EExpStats Stat)
{
	return ExpStats[Stat];
}

void UStatsComponent::PlusExp(float Value)
{
	int32 CurrentExp = ExpStats[EExpStats::CURRENT_EXP];
	int32 MaxExp = ExpStats[EExpStats::MAX_EXP];
	
	if(MaxExp > (CurrentExp + Value)) //레벨업 아님
	{
		ExpStats[EExpStats::CURRENT_EXP] += Value;
	}
	else //레벨업
	{
		LevelUp(CurrentExp, MaxExp, Value);
	}
}

void UStatsComponent::LevelUp(int32 CurrentExp, int32 MaxExp, int32 Value)
{
	int32 RemainExp = CurrentExp + Value - MaxExp;
	ExpStats[EExpStats::CURRENT_EXP] = RemainExp;
	ExpStats[EExpStats::LEVEL] += 1;
	CalcMaxExp();

	CurrentStats[EStats::SP] += 1;

	OnSavePlayerData.ExecuteIfBound(); //db update
	OnLevelUp.ExecuteIfBound();
}

void UStatsComponent::DevLevelUp()
{
	ExpStats[EExpStats::CURRENT_EXP] = 0;
	ExpStats[EExpStats::LEVEL] += 1;
	CalcMaxExp();
	CurrentStats[EStats::SP] += 1;

	OnSavePlayerData.ExecuteIfBound();
}

void UStatsComponent::UpdateSkillLevel(int32 SkillNum, int32 WeaponId)
{
	//db 업뎃
	FPlayerSkillInfoDB PlayerSkillInfo;
	PlayerSkillInfo.Pid = 9824;
	PlayerSkillInfo.Skillnum = SkillNum;
	PlayerSkillInfo.Weaponid = WeaponId;

	if(Http)
    {
        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

        
		Request->OnProcessRequestComplete().BindUObject(this, &ThisClass::OnSkillUpComplete);
		Request->SetURL("http://localhost:8080/api/SkillInfo/SkillUp");
        Request->SetVerb("PUT");
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        
        FString JsonString;
        FJsonObjectConverter::UStructToJsonObjectString(PlayerSkillInfo, JsonString);
        Request->SetContentAsString(JsonString);

        Request->ProcessRequest();


		FUpdateSPDB UpdateSPDB;
		UpdateSPDB.Pid = 9824;
		Request->SetURL("http://localhost:8080/api/PlayerInfo/MinusSP");
		FJsonObjectConverter::UStructToJsonObjectString(UpdateSPDB, JsonString);
		Request->SetContentAsString(JsonString);

        Request->ProcessRequest();
    }
	
}

void UStatsComponent::OnSkillUpComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
	if(Success)
	{
		UCombatComponent* CombatComp = Cast<UCombatComponent>(GetOwner()->GetComponentByClass(UCombatComponent::StaticClass()));
		if (CombatComp && CombatComp->GetEquippedWeapon())
		{
			CombatComp->GetEquippedWeapon()->InitSkillInfo();
		}
	}
}