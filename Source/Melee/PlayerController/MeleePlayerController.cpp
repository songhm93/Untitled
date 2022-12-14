#include "MeleePlayerController.h"
#include "JsonObjectConverter.h"

#include "../PlayerCharacter/BaseCharacter.h"
#include "../Type/Types.h."
#include "../Type/Stats.h"
#include "../Component/StatsComponent.h"
#include "../Component/CombatComponent.h"
#include "../Component/StateManagerComponent.h"
#include "../Component/InventoryComponent.h"
#include "../MeleeGameMode.h"
#include "../Widget/GetItemWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"


AMeleePlayerController::AMeleePlayerController()
{
    ExistGetItemWidgetNum = 0;
    WaitGetItemWidgetNum = 0;

    Http = &FHttpModule::Get();
}

void AMeleePlayerController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    BaseCharacter = Cast<ABaseCharacter>(InPawn);
}

void AMeleePlayerController::Tick(float DeltaTime)
{
    TrackingSprint();
}

void AMeleePlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    if(BaseCharacter)
    {
        StatComp = BaseCharacter->GetStatComp();
        if(StatComp)
        {
            StatComp->OnSavePlayerData.BindUObject(this, &ThisClass::SaveData);
        }
    }
    
}

void AMeleePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    if(nullptr == InputComponent) return; 
    InputComponent->BindAction("Attack", IE_Pressed, this, &ThisClass::AttackButtonPressed);
}

void AMeleePlayerController::AttackButtonPressed() 
{
   OnLightAttack.ExecuteIfBound();
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
        FPlayerInfoDB PlayerInfo = ConvertToPlayerInfo(Response->GetContentAsString());
        if(PlayerInfo.Isvalid)
        {
            SpawnLocaton = FVector(PlayerInfo.Xcoord, PlayerInfo.Ycoord, PlayerInfo.Zcoord);
            if(BaseCharacter && BaseCharacter->GetStatComp() && BaseCharacter->GetInventoryComp())
            {
                BaseCharacter->GetStatComp()->InitStats();
                BaseCharacter->GetStatComp()->SetCurrentStatValue(EStats::HP, PlayerInfo.Currenthp);
                BaseCharacter->GetStatComp()->SetCurrentStatValue(EStats::STAMINA, PlayerInfo.Currentstamina);
                BaseCharacter->GetStatComp()->SetCurrentStatValue(EStats::ATK, PlayerInfo.Atk);
                BaseCharacter->GetStatComp()->SetCurrentStatValue(EStats::DEF, PlayerInfo.Def);
                BaseCharacter->GetStatComp()->SetCurrentStatValue(EStats::SP, PlayerInfo.Sp);
                BaseCharacter->GetStatComp()->SetExpStatValue(EExpStats::LEVEL, PlayerInfo.Level);
                BaseCharacter->GetStatComp()->SetExpStatValue(EExpStats::CURRENT_EXP, PlayerInfo.Currentexp);
                BaseCharacter->GetStatComp()->CalcMaxExp();
                BaseCharacter->GetInventoryComp()->InitGold(PlayerInfo.Gold);
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
        TArray<FPlayerInventoryDB> PlayerInventory = ConvertToPlayerInventory(Response->GetContentAsString());
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

FPlayerInfoDB AMeleePlayerController::ConvertToPlayerInfo(const FString& ResponseString)
{
    FPlayerInfoDB PlayerInfo;
    if(!ResponseString.Contains("timestamp")) //테이블에 해당하는 PID가 있을 때
    {
        FJsonObjectConverter::JsonObjectStringToUStruct(*ResponseString, &PlayerInfo, 0, 0);
    }

    return PlayerInfo;
}

TArray<FPlayerInventoryDB> AMeleePlayerController::ConvertToPlayerInventory(const FString& ResponseString)
{
    TArray<FPlayerInventoryDB> PlayerInventory;
    if(!ResponseString.Contains("timestamp"))
    {
        FJsonObjectConverter::JsonArrayStringToUStruct(*ResponseString, &PlayerInventory, 0, 0);
    }

    return PlayerInventory;
}

void AMeleePlayerController::SaveData()
{
    FPlayerInfoDB PlayerInfo;
    PlayerInfo.Pid = 9824;
    if(BaseCharacter && BaseCharacter->GetStatComp() &&  BaseCharacter->GetInventoryComp())
    {
        PlayerInfo.Isvalid = true;
        PlayerInfo.Currenthp = BaseCharacter->GetStatComp()->GetCurrentStatValue(EStats::HP);
        PlayerInfo.Maxhp = BaseCharacter->GetStatComp()->GetMaxValue(EStats::HP);
        PlayerInfo.Currentstamina = BaseCharacter->GetStatComp()->GetCurrentStatValue(EStats::STAMINA);
        PlayerInfo.Maxstamina = BaseCharacter->GetStatComp()->GetMaxValue(EStats::STAMINA);
        PlayerInfo.Xcoord = BaseCharacter->GetActorLocation().X;
        PlayerInfo.Ycoord = BaseCharacter->GetActorLocation().Y;
        PlayerInfo.Zcoord = BaseCharacter->GetActorLocation().Z;
        PlayerInfo.Level = BaseCharacter->GetStatComp()->GetExpStatValue(EExpStats::LEVEL);
        PlayerInfo.Currentexp = BaseCharacter->GetStatComp()->GetExpStatValue(EExpStats::CURRENT_EXP);
        PlayerInfo.Maxexp = BaseCharacter->GetStatComp()->GetExpStatValue(EExpStats::MAX_EXP);
        PlayerInfo.Atk = BaseCharacter->GetStatComp()->GetCurrentStatValue(EStats::ATK);
        PlayerInfo.Def = BaseCharacter->GetStatComp()->GetCurrentStatValue(EStats::DEF);
        PlayerInfo.Gold = BaseCharacter->GetInventoryComp()->GetGold();
        PlayerInfo.Sp = BaseCharacter->GetStatComp()->GetCurrentStatValue(EStats::SP);
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

void AMeleePlayerController::GetItemWidgetVisible(FItemInfoInSlot AddItemInfo, int32 Amount)
{
    if(ExistGetItemWidgetNum == 5) //화면에 위젯이 5개 떠있으면 그 뒤에 떠야하는 정보를 저장 후 대기
    {
        GetItemWidgetQueue.Add(FGetItemQueue(AddItemInfo, Amount));
        ++WaitGetItemWidgetNum;
        return;
    }
	if(GetItemWidgetClass)
	{
		GetItemWidget = CreateWidget<UGetItemWidget>(GetWorld(), GetItemWidgetClass);
		if(GetItemWidget)
		{
			++ExistGetItemWidgetNum;
            GetItemWidget->AddToViewport();
            GetItemWidget->SetDesiredSizeInViewport(FVector2D(410, 60));
            GetItemWidget->SetPositionInViewport(FVector2D(1100, 400));
			GetItemWidget->SetVisibility(ESlateVisibility::Visible);
			GetItemWidget->Init(AddItemInfo, Amount);

            GetItemWidgetScrollUp();
		}
	}
}

void AMeleePlayerController::GetItemWidgetScrollUp()
{
    for(UGetItemWidget* WidgetLog : GetItemWidgetList)
    {
        if(WidgetLog)
        {
            UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(WidgetLog->GetItemWidgetBox);
            if(CanvasSlot)
            {
                FVector2D Position = CanvasSlot->GetPosition();
                FVector2D NewPosition(CanvasSlot->GetPosition().X, Position.Y - CanvasSlot->GetSize().Y);
                CanvasSlot->SetPosition(NewPosition);
            }
        }
    }
    GetItemWidgetList.Add(GetItemWidget);
}

void AMeleePlayerController::RemoveGetItemWidget()
{
    GetItemWidget = *(GetItemWidgetList.begin());
    GetItemWidget->RemoveFromParent();
    GetItemWidgetList.RemoveAt(0);
    --ExistGetItemWidgetNum;
    if(WaitGetItemWidgetNum > 0) //화면에 떠있던 위젯들 지워질 때 떠야할 대기중인 위젯이 있으면
    {
        GetItemWidgetVisible(GetItemWidgetQueue[0].ItemInfo, GetItemWidgetQueue[0].Amount);
        GetItemWidgetQueue.RemoveAt(0);
        --WaitGetItemWidgetNum;
    }
}