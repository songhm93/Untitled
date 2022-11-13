#include "InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "../Component/InventoryComponent.h"

void UInventorySlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

}

void UInventorySlotWidget::Init(FItemInfoInSlot ItemInfoInSlot, int32 SlotIndex)
{
    ItemInfo = ItemInfoInSlot;
    Index = SlotIndex;
}

void UInventorySlotWidget::UpdateSlot(bool IsValidItemInSlot)
{
    if(!IsValidItemInSlot)
    {
        if(SlotButton) //얘네가 왜 없는지
        {
            SlotButton->SetIsEnabled(false);
        }
        if(ItemIcon)
            ItemIcon->SetVisibility(ESlateVisibility::Hidden);
        if(ItemAmount)
            ItemAmount->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        if(SlotButton)
            SlotButton->SetIsEnabled(true);
        if(ItemIcon){
            ItemIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
            ItemIcon->SetBrushFromTexture(ItemInfo.Icon);
        }
        if(ItemAmount)
        {
            ItemAmount->SetText(FText::FromString(FString::FromInt(ItemInfo.Amount)));
            if(ItemInfo.CanStack)
                ItemAmount->SetVisibility(ESlateVisibility::HitTestInvisible);
            else
                ItemAmount->SetVisibility(ESlateVisibility::Hidden);
        }
        
    }
    
}
