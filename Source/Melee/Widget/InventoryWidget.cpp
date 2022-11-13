#include "InventoryWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"

#include "../Component/InventoryComponent.h"
#include "InventorySlotWidget.h"


void UInventoryWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    SlotsPerRow = 5;
}

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();


    if(GetOwningPlayerPawn())
        InventoryComp = Cast<UInventoryComponent>(GetOwningPlayerPawn()->GetComponentByClass(UInventoryComponent::StaticClass()));
    if(InventoryComp)
    {
        InventoryComp->OnGenerateSlotWidget.BindUObject(this, &ThisClass::GenerateSlotWidgets);
        InventoryComp->OnVisibleInventory.BindUObject(this, &ThisClass::VisibleWidget);
    }
    if(CloseButton)
    {
        CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseInventory);
    }
    SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryWidget::GenerateSlotWidgets()
{   
    if(SlotPanel)
    {
        SlotPanel->ClearChildren();
        SlotWidgets.Empty(50);
        if(InventoryComp && InventorySlotWidgetClass)
        {
            //슬롯 하나씩 생성
            //슬롯에 아이템 정보와 인덱스를 넘김

            for(int i = 0; i < InventoryComp->GetTotalSlotNum(); ++i)
            {
                //총 슬롯 갯수만큼 위젯 생성을 해야하고
                // Init은 아이템 있는 자리만 해야한다.
                UUserWidget* InventorySlotWidget = CreateWidget<UInventorySlotWidget>(GetWorld(), InventorySlotWidgetClass);

                SlotWidgets.Add(InventorySlotWidget);
                
                SlotPanel->AddChildToUniformGrid(InventorySlotWidget, i / SlotsPerRow, i % SlotsPerRow);
                

                if(InventoryComp->GetInventorySlots().IsValidIndex(i))
                {
                    Cast<UInventorySlotWidget>(InventorySlotWidget)->Init(InventoryComp->GetInventorySlots()[i], i);
                    Cast<UInventorySlotWidget>(InventorySlotWidget)->UpdateSlot(true);
    
                }  
                else
                {
                    Cast<UInventorySlotWidget>(InventorySlotWidget)->UpdateSlot(false);
                }
            }
        }
    }
}

void UInventoryWidget::VisibleWidget(bool Visible)
{ 
    if(Visible)
    {
        SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        SetVisibility(ESlateVisibility::Hidden);
    }
}

void UInventoryWidget::CloseInventory()
{
    SetVisibility(ESlateVisibility::Hidden);
    if(InventoryComp)
        InventoryComp->SetIsVisible(false);
}