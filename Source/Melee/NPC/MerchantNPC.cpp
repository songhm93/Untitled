#include "MerchantNPC.h"
#include "Engine/DataTable.h"

#include "../PlayerCharacter/BaseCharacter.h"
#include "../PlayerController/MeleePlayerController.h"
#include "../Widget/MainHUDWidget.h"
#include "../Widget/MerchantWidget.h"

AMerchantNPC::AMerchantNPC()
{

}

void AMerchantNPC::Interact(AActor* Caller)
{
    Super::Interact(Caller);

    //여기서 플레이어 MainHUD 지우고, 상점 위젯 띄우기.
    ABaseCharacter* MC = Cast<ABaseCharacter>(Caller);
    
    if(MC && MC->GetMainHUDWidget())
    {
        AMeleePlayerController* MPC = Cast<AMeleePlayerController>(MC->GetController());
        if(MPC && MPC->GetMerchantWidgetClass())
        {
            MC->GetMainHUDWidget()->SetVisibility(ESlateVisibility::Hidden);
            UMerchantWidget* MerchantWidget = CreateWidget<UMerchantWidget>(GetWorld(), MPC->GetMerchantWidgetClass());
            if(MerchantWidget)
            {
                //Init할때 이 NPC가 갖고있는 물품들을 Init에 넣어 보내줌
                MPC->SetMerchantWidget(MerchantWidget);
                TArray<FItemInfoInSlot> HaveItemsInfo;
                FString TablePath = FString(TEXT("/Game/CombatSystem/DataTable/ItemInfo"));
                UDataTable* ItemInfoTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *TablePath));
                if(ItemInfoTableObject)
                {
                    FItemInfoTable* ItemInfoRow = nullptr;
                    for(auto ItemId : HaveItemId)
                    {
                        ItemInfoRow = ItemInfoTableObject->FindRow<FItemInfoTable>(FName(FString::FromInt(ItemId)), TEXT(""));
                        if(ItemInfoRow)
                        {
                            HaveItemsInfo.Add(FItemInfoInSlot(0, ItemId, 1, ItemInfoRow->Name, ItemInfoRow->Desc, ItemInfoRow->Icon, ItemInfoRow->Usetext, ItemInfoRow->Canuse, ItemInfoRow->Canstack, ItemInfoRow->Category, ItemInfoRow->Isactor, 0, ItemInfoRow->BuyGold, ItemInfoRow->SellGold));
                        }
                    }
                }
                MerchantWidget->Init(HaveItemsInfo);
                MerchantWidget->AddToViewport();
            }
        }
        
    }
}

void AMerchantNPC::BeginPlay()
{
    Super::BeginPlay();
}