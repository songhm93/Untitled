#include "BaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "JsonObjectConverter.h"

#include "../Type/Types.h"
#include "../Component/StateManagerComponent.h"

ABaseWeapon::ABaseWeapon()
{
    SetEquipmentType(EEquipmentType::WEAPON);
    CurrentElement = EElements::NONE;
    WeaponId = 0;
}

void ABaseWeapon::SimulateWeaponPhysics()
{
    GetItemMeshComp()->SetCollisionProfileName(TEXT("PhysicsActor"));
    GetItemMeshComp()->SetSimulatePhysics(true);
    GetItemMeshComp()->SetMassOverrideInKg(NAME_None, 350.f);
    GetItemMeshComp()->SetLinearDamping(-0.5f);
    GetItemMeshComp()->SetAngularDamping(5.f);
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
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("스킬 정보 가져오기 실패..."));
    }
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
