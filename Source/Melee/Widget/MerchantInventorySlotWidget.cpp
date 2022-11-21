#include "MerchantInventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "../Component/InventoryComponent.h"
#include "InfoWidget.h"

void UMerchantInventorySlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    Index = -1;
}

void UMerchantInventorySlotWidget::Init(FItemInfoInSlot ItemInfoInSlot, int32 SlotIndex)
{
    ItemInfo = ItemInfoInSlot;
    Index = SlotIndex;
}

void UMerchantInventorySlotWidget::UpdateSlot(bool IsValidItemInSlot)
{
    if(!IsValidItemInSlot)
    {
        if(SlotButton)
        {
            SlotButton->SetIsEnabled(false);
        }
        if(ItemIcon)
            ItemIcon->SetVisibility(ESlateVisibility::Hidden);
        if(ItemAmount)
            ItemAmount->SetVisibility(ESlateVisibility::Hidden);
        if(XText)
            XText->SetVisibility(ESlateVisibility::Hidden);
        if(InfoWidget)
        {
            if(SlotButton)
            {
                SlotButton->SetToolTip(InfoWidget);
            }
        }
    }
    else
    {
        if(SlotButton)
            SlotButton->SetIsEnabled(true);
        if(ItemIcon)
        {
            ItemIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
            ItemIcon->SetBrushFromTexture(ItemInfo.Icon);
        }
        if(ItemAmount)
        {
            ItemAmount->SetText(FText::FromString(FString::FromInt(ItemInfo.Amount)));
            if(ItemInfo.CanStack)
            {
                ItemAmount->SetVisibility(ESlateVisibility::HitTestInvisible);
                XText->SetVisibility(ESlateVisibility::HitTestInvisible);
            }
            else
            {
                ItemAmount->SetVisibility(ESlateVisibility::Hidden);
                XText->SetVisibility(ESlateVisibility::Hidden);
            }  
        }
        if(ItemEquipped)
        {
            if(ItemInfo.Equipped)
                ItemEquipped->SetVisibility(ESlateVisibility::Visible);
            else
                ItemEquipped->SetVisibility(ESlateVisibility::Hidden);
        }

        if(InfoWidget)
        {
            InfoWidget->Init(ItemInfo);
            if(SlotButton)
            {
                SlotButton->SetToolTip(InfoWidget);
            }
        }
        else
        {
            InfoWidget = CreateWidget<UInfoWidget>(this, InfoWidgetClass);
            if(InfoWidget)
            {
                InfoWidget->Init(ItemInfo);
                if(SlotButton)
                {
                    SlotButton->SetToolTip(InfoWidget);
                }
            }
        }
    }
}