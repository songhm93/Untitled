#include "GetItemWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "../PlayerController/MeleePlayerController.h"



void UGetItemWidget::Init(FItemInfoInSlot ItemInfoInSlot, int32 _Amount)
{
    ItemInfo = ItemInfoInSlot;
    Amount = _Amount;
    Update();
}

void UGetItemWidget::FadeIn()
{
    if(FadeInAnim)
    {
        PlayAnimation(FadeInAnim);
    }
}

void UGetItemWidget::FadeOut()
{
    if(FadeOutAnim)
    {
        PlayAnimation(FadeOutAnim);
        GetWorld()->GetTimerManager().SetTimer(EndFadeOutAnimTimerHandle, this, &ThisClass::EndFadeOutAnim, FadeOutAnim->GetEndTime());
    }
}

void UGetItemWidget::Update()
{
    SetVisibility(ESlateVisibility::Visible);
    if(ItemName)
    {
        ItemName->SetText(FText::FromString(ItemInfo.ItemName));
    }
    if(ItemAmount)
    {
        ItemAmount->SetText(FText::FromString(FString::FromInt(Amount)));
    }
    if(ItemIcon)
    {
        ItemIcon->SetBrushFromTexture(ItemInfo.Icon);
    }
    FadeIn();
    GetWorld()->GetTimerManager().SetTimer(FadeOutTimerHandle, this, &ThisClass::FadeOut, FadeOutTime);
}

void UGetItemWidget::EndFadeOutAnim()
{
    AMeleePlayerController* MPC = Cast<AMeleePlayerController>(GetOwningPlayer());
    if(MPC)
    {
        MPC->RemoveGetItemWidget();
    }
}