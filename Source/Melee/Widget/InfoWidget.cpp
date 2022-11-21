#include "InfoWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "../Interface/ItemInterface.h"
#include "../Equipment/BaseWeapon.h"
#include "../Equipment/BaseArmor.h"

void UInfoWidget::Init(FItemInfoInSlot ItemInfoInSlot)
{
    ItemInfo = ItemInfoInSlot;
    UpdateInfo();
}

void UInfoWidget::UpdateInfo()
{
    if(ItemIcon)
    {
        ItemIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
        ItemIcon->SetBrushFromTexture(ItemInfo.Icon);
    }
    if(ItemName)
    {
        ItemName->SetText(FText::FromString(ItemInfo.ItemName));
    }
    if(Category)
    {
        switch(ItemInfo.Category)
        {
            case EItemCategory::CONSUMEABLE:
                Category->SetText(FText::FromString(TEXT("소모품")));
                break;
            case EItemCategory::WEAPON:
                Category->SetText(FText::FromString(TEXT("무기")));
                break;
            case EItemCategory::HELMET:
                Category->SetText(FText::FromString(TEXT("방어구")));
                break;
            case EItemCategory::CHEST:
                Category->SetText(FText::FromString(TEXT("방어구")));
                break;
            case EItemCategory::GAUNTLET:
                Category->SetText(FText::FromString(TEXT("방어구")));
                break;
            case EItemCategory::BOOT:
                Category->SetText(FText::FromString(TEXT("방어구")));
                break;
            case EItemCategory::QUEST:
                Category->SetText(FText::FromString(TEXT("퀘스트")));
                break;
            case EItemCategory::READABLES:
                Category->SetText(FText::FromString(TEXT("기타")));
                break;
        }
    }
    if(Info)
    {
        Info->SetText(FText::FromString(ItemInfo.Desc));
    }
}
