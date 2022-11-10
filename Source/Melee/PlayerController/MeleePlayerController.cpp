#include "MeleePlayerController.h"
#include "JsonObjectConverter.h"
#include "Blueprint/UserWidget.h"

#include "../PlayerCharacter/BaseCharacter.h"
#include "../Type/Types.h."
#include "../Type/Stats.h"
#include "../Component/StatsComponent.h"
#include "../Component/CombatComponent.h"
#include "../Component/StateManagerComponent.h"
#include "../Component/InventoryComponent.h"
#include "../MeleeGameMode.h"

AMeleePlayerController::AMeleePlayerController()
{
    bLeftClickIsPressed = false;
    LeftClickTime = 0.f;
    ChargedTime = 0.3f;
    bCharged = false;

    Http = &FHttpModule::Get();
}

void AMeleePlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    BaseCharacter = Cast<ABaseCharacter>(InPawn);
    UE_LOG(LogTemp, Warning, TEXT("빙의!"));
}

void AMeleePlayerController::Tick(float DeltaTime)
{
    TrackingChargedAttack(DeltaTime);
    TrackingSprint();
}

void AMeleePlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    if(MainHUDClass)
    {
        MainHUDWidget = CreateWidget<UUserWidget>(this, MainHUDClass);
        if(MainHUDWidget)
        {
            MainHUDWidget->AddToViewport();
        }
    }
}

void AMeleePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    if(nullptr == InputComponent) return; 
    InputComponent->BindAction("Attack", IE_Pressed, this, &ThisClass::AttackButtonPressed);
    InputComponent->BindAction("Attack", IE_Released, this, &ThisClass::AttackButtonReleased);
}
 
void AMeleePlayerController::AttackButtonPressed() 
{
    bLeftClickIsPressed = true;
}

void AMeleePlayerController::AttackButtonReleased() 
{
    bLeftClickIsPressed = false; 
    //if(BaseCharacter && !bCharged)
        //BaseCharacter->LightAttack();
    if(!bCharged)
        OnLightAttack.ExecuteIfBound();
    bCharged = false;
}

void AMeleePlayerController::TrackingChargedAttack(float DeltaTime)
{
    if(bLeftClickIsPressed)
    {
        LeftClickTime += DeltaTime;
        if(LeftClickTime >= ChargedTime)
        {
            OnChargedAttack.ExecuteIfBound();
            bCharged = true;
            bLeftClickIsPressed = false;
        }
    }
    else
    {
        LeftClickTime = 0.f;
    }  
}

void AMeleePlayerController::TrackingSprint()
{
    bool Condition = 
        BaseCharacter && 
        BaseCharacter->GetStatComp() && 
        BaseCharacter->GetStateManagerComp() && 
        BaseCharacter->GetStateManagerComp()->GetMovementType() == EMovementType::SPRINTING;
    if(Condition)
    {   //현재 스태미너가 소모량보다 적은 경우, 달리지 않는데 쉬프트가 눌려 있는 경우
        if((BaseCharacter->GetStatComp()->GetCurrentStatValue(EStats::STAMINA) < BaseCharacter->GetSprintStaminaCost()) || BaseCharacter->GetVelocity().Size() <= 0.f)
        {
            BaseCharacter->SetMovementType(EMovementType::JOGGING);
        }
        BaseCharacter->GetStatComp()->PlusCurrentStatValue(EStats::STAMINA, -(BaseCharacter->GetSprintStaminaCost()));
    }
}

void AMeleePlayerController::RequestEntry()
{
    FString PID = "9824"; //얘를 어떤식으로?
    if(Http)
    {
        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

        Request->OnProcessRequestComplete().BindUObject(this, &AMeleePlayerController::OnPlayerInfoRequestComplete);
        Request->SetURL("http://localhost:8080/api/PlayerInfo/" + PID);
        Request->SetVerb("GET");
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        
        Request->ProcessRequest();
    }
}

void AMeleePlayerController::OnPlayerInfoRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
	FVector SpawnLocaton = FVector(1455.f, 1975.f, 400.f);
    if(Success) //요청 성공.
	{
        FPlayerInfo PlayerInfo = ConvertToPlayerInfo(Response->GetContentAsString());
        if(PlayerInfo.Isvalid)
        {
            SpawnLocaton = FVector(PlayerInfo.Xcoord, PlayerInfo.Ycoord, PlayerInfo.Zcoord);
            if(BaseCharacter && BaseCharacter->GetStatComp())
            {
                BaseCharacter->GetStatComp()->InitStats();
                BaseCharacter->GetStatComp()->SetCurrentStatValue(EStats::HP, PlayerInfo.Currenthp);
                BaseCharacter->GetStatComp()->SetCurrentStatValue(EStats::STAMINA, PlayerInfo.Currentstamina);
                BaseCharacter->GetStatComp()->SetCurrentStatValue(EStats::ATK, PlayerInfo.Atk);
                BaseCharacter->GetStatComp()->SetCurrentStatValue(EStats::DEF, PlayerInfo.Def);
            }
        }
	}
	else //요청 실패
	{
		UE_LOG(LogTemp, Warning, TEXT("요청 실패"));
	}

    if(GetPawn())
    {
        GetPawn()->SetActorLocation(SpawnLocaton);
    }
    //플레이어가 가지고 있는 아이템가져와서 인벤토리 컴포넌트에 
    //플레이어의 정보 가지고 와서 스탯 컴포넌트에

    if(Request->OnProcessRequestComplete().IsBound())
    {
        Request->OnProcessRequestComplete().Unbind();
        Request->OnProcessRequestComplete().BindUObject(this, &AMeleePlayerController::OnInventoryRequestComplete);
    }
    FString PID = "9824"; //얘를 어떤식으로?
    Request->SetURL("http://localhost:8080/api/PlayerInventory/" + PID);
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    
    Request->ProcessRequest(); 
}

void AMeleePlayerController::OnInventoryRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success)
{
    if(Success)
    {
        UE_LOG(LogTemp, Warning, TEXT("인벤토리 가져오기 성공!"));
        TArray<FPlayerInventory> PlayerInventory = ConvertToPlayerInventory(Response->GetContentAsString());
        if(!PlayerInventory.IsEmpty())
        {
            if(BaseCharacter && BaseCharacter->GetInventoryComp())
            {
                BaseCharacter->GetInventoryComp()->InitInventory(PlayerInventory);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("인벤토리 가져오기 실패..."));
    }
}

FPlayerInfo AMeleePlayerController::ConvertToPlayerInfo(const FString& ResponseString)
{
    FPlayerInfo PlayerInfo;
    if(!ResponseString.Contains("timestamp")) //테이블에 해당하는 PID가 있을 때
    {
        FJsonObjectConverter::JsonObjectStringToUStruct(*ResponseString, &PlayerInfo, 0, 0);
    }

    return PlayerInfo;
}

TArray<FPlayerInventory> AMeleePlayerController::ConvertToPlayerInventory(const FString& ResponseString)
{
    TArray<FPlayerInventory> PlayerInventory;
    if(!ResponseString.Contains("timestamp"))
    {
        FJsonObjectConverter::JsonArrayStringToUStruct(*ResponseString, &PlayerInventory, 0, 0);
    }

    return PlayerInventory;
}

void AMeleePlayerController::SaveData()
{
    FPlayerInfo PlayerInfo;
    PlayerInfo.Pid = 9824;
    if(BaseCharacter && BaseCharacter->GetStatComp())
    {
        PlayerInfo.Isvalid = true;
        PlayerInfo.Currenthp = BaseCharacter->GetStatComp()->GetCurrentStatValue(EStats::HP);
        PlayerInfo.Maxhp = BaseCharacter->GetStatComp()->GetMaxValue(EStats::HP);
        PlayerInfo.Currentstamina = BaseCharacter->GetStatComp()->GetCurrentStatValue(EStats::STAMINA);
        PlayerInfo.Maxstamina = BaseCharacter->GetStatComp()->GetMaxValue(EStats::STAMINA);
        PlayerInfo.Xcoord = BaseCharacter->GetActorLocation().X;
        PlayerInfo.Ycoord = BaseCharacter->GetActorLocation().Y;
        PlayerInfo.Zcoord = BaseCharacter->GetActorLocation().Z;
        PlayerInfo.Level = 1;
        PlayerInfo.Currentexp = 0.f;
        PlayerInfo.Maxexp = 100.f;
        PlayerInfo.Atk = BaseCharacter->GetStatComp()->GetMaxValue(EStats::ATK);
        PlayerInfo.Def = BaseCharacter->GetStatComp()->GetMaxValue(EStats::DEF);
    }

    if(Http)
    {
        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

        Request->SetURL("http://localhost:8080/api/PlayerInfo/");
        Request->SetVerb("PUT");
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        
        FString JsonString;
        FJsonObjectConverter::UStructToJsonObjectString(PlayerInfo, JsonString);
        Request->SetContentAsString(JsonString);

        Request->ProcessRequest();
    }
}