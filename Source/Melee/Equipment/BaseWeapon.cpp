#include "BaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"

#include "../Type/Types.h"
#include "../Component/StateManagerComponent.h"
#include "../Component/StatsComponent.h"
#include "../PlayerCharacter/BaseCharacter.h"

ABaseWeapon::ABaseWeapon()
{
    SetEquipmentType(EEquipmentType::WEAPON);
    CurrentElement = EElements::NONE;
    WeaponId = 0;
}


void ABaseWeapon::Skill1()
{
    UStateManagerComponent* StateManager = Cast<UStateManagerComponent>(GetOwner()->GetComponentByClass(UStateManagerComponent::StaticClass()));
    if(StateManager)
    {
        StateManager->SetCurrentState(ECurrentState::ATTACKING);
    }
}

void ABaseWeapon::Skill2()
{
    UStateManagerComponent* StateManager = Cast<UStateManagerComponent>(GetOwner()->GetComponentByClass(UStateManagerComponent::StaticClass()));
    if(StateManager)
    {
        StateManager->SetCurrentState(ECurrentState::ATTACKING);
    }
}

void ABaseWeapon::Skill3()
{
    UStateManagerComponent* StateManager = Cast<UStateManagerComponent>(GetOwner()->GetComponentByClass(UStateManagerComponent::StaticClass()));
    if(StateManager)
    {
        StateManager->SetCurrentState(ECurrentState::ATTACKING);
    }
}

void ABaseWeapon::SkillUltimate()
{
    UStateManagerComponent* StateManager = Cast<UStateManagerComponent>(GetOwner()->GetComponentByClass(UStateManagerComponent::StaticClass()));
    if(StateManager)
    {
        StateManager->SetCurrentState(ECurrentState::ATTACKING);
    }
}

void ABaseWeapon::InitSkillInfo()
{
    GetSkillInfoDB();
}

void ABaseWeapon::GetSkillInfoDB()
{
    FString PID = "9824";

    if(Http)
    {
        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
        Request->OnProcessRequestComplete().BindUObject(this, &ABaseWeapon::OnSkillInfoRequestComplete);
        Request->SetURL("http://localhost:8080/api/SkillInfo/" + PID + "/" + FString::FromInt(WeaponId));
        Request->SetVerb("GET");
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        
        Request->ProcessRequest();
    }
}

void ABaseWeapon::OnSkillInfoRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
    if(Success)
    {
        TArray<FPlayerSkillInfo> PlayerSkillInfo = ConvertToPlayerSkillInfo(Response->GetContentAsString());
        if(!PlayerSkillInfo.IsEmpty() && !SkillInfo.IsEmpty())
        {
            for(FPlayerSkillInfo Info : PlayerSkillInfo)
            {
                SkillInfo[Info.Skillnum].CurrentLevel = Info.Skilllevel;
            }
            InitSkillDT();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("스킬 정보 가져오기 실패..."));
    }
}

void ABaseWeapon::InitSkillDT()
{
}

TArray<FPlayerSkillInfo> ABaseWeapon::ConvertToPlayerSkillInfo(const FString& ResponseString)
{
    TArray<FPlayerSkillInfo> PlayerSkillInfo;

    if(!ResponseString.Contains("timestamp"))
    {
        FJsonObjectConverter::JsonArrayStringToUStruct(*ResponseString, &PlayerSkillInfo, 0, 0);
    }

    return PlayerSkillInfo;
}

float ABaseWeapon::GetPlayerATK()
{
    ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
    float PlayerATK = 0.f;
    if(OwnerCharacter && OwnerCharacter->GetStatComp())
        PlayerATK = OwnerCharacter->GetStatComp()->GetCurrentStatValue(EStats::ATK);

    return PlayerATK;
}

int32 ABaseWeapon::SkillATKCalc(int32 SkillNum)
{
    if(GetSkillInfo().Contains(SkillNum))
    {
        return GetSkillInfo()[SkillNum].CurrentLevel;
    }
    return 1.f;
}

int32 ABaseWeapon::GetWeaponSkillATK(int32 SkillNum)
{
    switch (SkillNum)
    {
    case 1:
        return Skill1ATK;
    case 2:
        return Skill2ATK;
    case 3:
        return Skill3ATK;
    case 4:
        return Skill4ATK;
    }
    return 1;
}