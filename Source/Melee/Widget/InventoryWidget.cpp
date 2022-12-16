#include "InventoryWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Components/WidgetComponent.h"
#include "Components/UniformGridSlot.h"
#include "Components/ScrollBox.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "../Component/InventoryComponent.h"
#include "InventorySlotWidget.h"
#include "InventoryActionMenuWidget.h"
#include "../PlayerCharacter/BaseCharacter.h"
#include "ItemThrowWidget.h"
#include "MainHUDWidget.h"

void UInventoryWidget::NativePreConstruct()
{
    Super::NativePreConstruct();
    SlotsPerRow = 5;
}

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (GetOwningPlayerPawn())
        InventoryComp = Cast<UInventoryComponent>(GetOwningPlayerPawn()->GetComponentByClass(UInventoryComponent::StaticClass()));
    if (InventoryComp)
    {
        InventoryComp->OnGenerateSlotWidget.BindUObject(this, &ThisClass::GenerateSlotWidgets);
        InventoryComp->OnVisibleInventory.BindUObject(this, &ThisClass::VisibleWidget);
    }
    if (CloseButton)
    {
        CloseButton->OnClicked.AddDynamic(this, &ThisClass::CloseInventory);
    }
    SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryWidget::GenerateSlotWidgets() //슬롯들 업데이트
{
    if (SlotPanel && CurrentGold)
    {
        SlotPanel->ClearChildren();
        SlotWidgets.Empty(50);
        if (InventoryComp && InventorySlotWidgetClass)
        {
            CurrentGold->SetText(FText::FromString(FString::FromInt(InventoryComp->GetGold())));
            //슬롯 하나씩 생성
            //슬롯에 아이템 정보와 인덱스를 넘김
            for (int i = 0; i < InventoryComp->GetTotalSlotNum(); ++i)
            {
                //총 슬롯 갯수만큼 위젯 생성을 해야하고
                //Init은 아이템 있는 자리만 해야한다.
                UUserWidget *InventorySlotWidget = CreateWidget<UInventorySlotWidget>(GetWorld(), InventorySlotWidgetClass);
                SlotWidgets.Add(InventorySlotWidget);

                UUniformGridSlot *GridSlot = SlotPanel->AddChildToUniformGrid(SlotWidgets[i], i / SlotsPerRow, i % SlotsPerRow);

                if (InventoryComp->GetInventorySlots().IsValidIndex(i))
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
    if (Visible)
    {
        GenerateSlotWidgets();
        SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        SetVisibility(ESlateVisibility::Hidden);
        if (InventoryActionMenuWidget)
        {
            InventoryActionMenuWidget->SetVisibility(ESlateVisibility::Hidden);
        }
        HideThrowWidget();
    }
}

void UInventoryWidget::CloseInventory()
{
    SetVisibility(ESlateVisibility::Hidden);
    if (InventoryComp)
        InventoryComp->SetIsVisible(false);
    if (InventoryActionMenuWidget)
    {
        InventoryActionMenuWidget->SetVisibility(ESlateVisibility::Hidden);
    }
    HideThrowWidget();
}

void UInventoryWidget::SlotClicked(int32 Index)
{
    UUniformGridSlot *GridSlot = UWidgetLayoutLibrary::SlotAsUniformGridSlot(SlotWidgets[Index]);

    if (GridSlot && InventoryScrollBox)
    {
        int32 Row = GridSlot->Row * 87;
        int32 Column = GridSlot->Column * 80;
        float Value = FMath::Clamp(Row - (InventoryScrollBox->GetScrollOffset() / 1.5), 0, 350);
        FVector2D Location = FVector2D(Column, Value);

        if (InventoryActionMenuWidget && InventoryComp)
        {
            InventoryActionMenuWidget->SetRenderTranslation(Location);
            InventoryActionMenuWidget->Init(InventoryComp->GetInventorySlots()[Index], Index);
        }
    }
}

void UInventoryWidget::HideThrowWidget()
{
    ABaseCharacter *OwnerCharacter = Cast<ABaseCharacter>(GetOwningPlayerPawn());
    if (OwnerCharacter)
    {
        UMainHUDWidget *MainHUD = OwnerCharacter->GetMainHUDWidget();
        if (MainHUD)
        {
            UItemThrowWidget *ItemThrowWidget = MainHUD->GetItemThrowWidget();
            if (ItemThrowWidget)
            {
                ItemThrowWidget->SetVisibility(ESlateVisibility::Hidden);
            }
        }
    }
}