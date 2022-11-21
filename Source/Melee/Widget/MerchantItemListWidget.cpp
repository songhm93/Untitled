#include "MerchantItemListWidget.h"
#include "Components/Button.h"

#include "ItemBuyWidget.h"


void UMerchantItemListWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UMerchantItemListWidget::Init(FItemInfoInSlot Info)
{
    ItemInfo = Info;
}


