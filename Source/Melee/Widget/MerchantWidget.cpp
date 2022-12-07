#include "MerchantWidget.h"
#include "Components/ListView.h"
#include "Components/UniformGridSlot.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "MerchantItemListWidget.h"
#include "MerchantInventorySlotWidget.h"
#include "../Component/InventoryComponent.h"
#include "../PlayerCharacter/BaseCharacter.h"
#include "../Widget/MainHUDWidget.h"



void UMerchantWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if(ExitButton)
    {
        ExitButton->OnClicked.AddDynamic(this, &ThisClass::ExitMerchant);
    }

    InventoryComp = Cast<UInventoryComponent>(GetOwningPlayerPawn()->GetComponentByClass(UInventoryComponent::StaticClass()));
    if(InventoryComp)
    {
        InventoryComp->OnGenerateMerchantSlot.BindUObject(this, &ThisClass::GenerateInventorySlots);
    }
}

void UMerchantWidget::Init(TArray<FItemInfoInSlot> Info) //이걸 호출했을 때 상점의 물품 정보가 일로.
{
    ItemsInfo = Info;
    Update();
}

void UMerchantWidget::Update()
{
    if(ItemListWidgetClass)
    {
        for(auto Item : ItemsInfo)
        {
            UMerchantItemListWidget* MerchantItemListWidget = CreateWidget<UMerchantItemListWidget>(GetWorld(), ItemListWidgetClass);
            
            if(MerchantItemListWidget)
            {
                MerchantItemListWidget->Init(Item);
                ItemListView->AddItem(MerchantItemListWidget); 
            }   
        }                                                     
    }
    
    GenerateInventorySlots();
}

void UMerchantWidget::GenerateInventorySlots()
{
    if(SlotPanel)
    {
        SlotPanel->ClearChildren();
        SlotWidgets.Empty(50);
        if(GetOwningPlayerPawn())
        {
            InventoryComp = Cast<UInventoryComponent>(GetOwningPlayerPawn()->GetComponentByClass(UInventoryComponent::StaticClass()));
        }
        SlotsPerRow = 6;
        if(InventoryComp && InventorySlotWidgetClass && CurrentGold)
        {
            CurrentGold->SetText(FText::FromString(FString::FromInt(InventoryComp->GetGold())));
            for(int i = 0; i < InventoryComp->GetTotalSlotNum(); ++i)
            {
                UUserWidget* InventorySlotWidget = CreateWidget<UMerchantInventorySlotWidget>(GetWorld(), InventorySlotWidgetClass);
                if(InventorySlotWidget)
                {
                    SlotWidgets.Add(InventorySlotWidget);
                    
                    UUniformGridSlot* GridSlot = SlotPanel->AddChildToUniformGrid(SlotWidgets[i], i / SlotsPerRow, i % SlotsPerRow);

                    if(InventoryComp->GetInventorySlots().IsValidIndex(i))
                    {
                        Cast<UMerchantInventorySlotWidget>(InventorySlotWidget)->Init(InventoryComp->GetInventorySlots()[i], i);
                        Cast<UMerchantInventorySlotWidget>(InventorySlotWidget)->UpdateSlot(true);
        
                    }  
                    else
                    {
                        Cast<UMerchantInventorySlotWidget>(InventorySlotWidget)->UpdateSlot(false);
                    }
                }
            }
        }
    }
}

void UMerchantWidget::ExitMerchant()
{
    ABaseCharacter* MC = Cast<ABaseCharacter>(GetOwningPlayerPawn());
    if(MC && MC->GetMainHUDWidget())
    {
        MC->GetMainHUDWidget()->SetVisibility(ESlateVisibility::Visible);
        RemoveFromViewport();
    }
    
}