#include "DraggedItemWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void UDraggedItemWidget::Init(FItemInfoInSlot ItemInfoInSlot, int32 SlotIndex)
{
    ItemInfo = ItemInfoInSlot;
    Index = SlotIndex;
    UpdateSlot();
}

void UDraggedItemWidget::UpdateSlot()
{
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
}