#include "InventoryActionMenuWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/UniformGridSlot.h"

#include "../Component/InventoryComponent.h"
#include "../PlayerCharacter/BaseCharacter.h"
#include "ItemThrowWidget.h"
#include "MainHUDWidget.h"
#include "InventoryWidget.h"


void UInventoryActionMenuWidget::NativeConstruct()
{
    if(UseButton)
    {
        UseButton->OnClicked.AddDynamic(this, &ThisClass::UseButtonClicked);
    }
    if(CancelButton)
    {   
        CancelButton->OnClicked.AddDynamic(this, &ThisClass::CancelButtonClicked);
    }
    if(DevideButton)
    {   
        DevideButton->OnClicked.AddDynamic(this, &ThisClass::DevideButtonClicked);
    }
    if(ThrowButton)
    {   
        ThrowButton->OnClicked.AddDynamic(this, &ThisClass::ThrowButtonClicked);
    }
    if(GetOwningPlayerPawn())
        InventoryComp = Cast<UInventoryComponent>(GetOwningPlayerPawn()->GetComponentByClass(UInventoryComponent::StaticClass()));
    
}

void UInventoryActionMenuWidget::Init(FItemInfoInSlot ItemInfoInSlot, int32 SlotIndex)
{
    ItemInfo = ItemInfoInSlot;
    Index = SlotIndex;
    Update();
}

void UInventoryActionMenuWidget::Update() //위젯을 띄우고 정보 업데이트
{
    SetVisibility(ESlateVisibility::Visible);

    if(UseText && UseButton && DevideButton && ThrowButton)
    {
        if(ItemInfo.CanUse)
        {
            if(ItemInfo.Equipped && ItemInfo.IsActor)
            {
                UseText->SetText(FText::FromString(TEXT("장착중")));
                ThrowButton->SetVisibility(ESlateVisibility::Collapsed);
                UseButton->SetIsEnabled(false);
            }
            else if(ItemInfo.Equipped && !ItemInfo.IsActor)
            {
                UseText->SetText(FText::FromString(TEXT("장착 해제")));
                UseButton->SetIsEnabled(true);
                ThrowButton->SetVisibility(ESlateVisibility::Collapsed);
                UseButton->SetVisibility(ESlateVisibility::Visible);
            }
            else
            {
                UseText->SetText(FText::FromString(ItemInfo.UseText));
                UseButton->SetIsEnabled(true);
                ThrowButton->SetVisibility(ESlateVisibility::Visible);
                UseButton->SetVisibility(ESlateVisibility::Visible);
            }
        }
        else
        {
            UseButton->SetVisibility(ESlateVisibility::Collapsed);
        }
        if(ItemInfo.CanStack && ItemInfo.Amount > 1)
        {
            DevideButton->SetVisibility(ESlateVisibility::Visible);
        }
        else if(!ItemInfo.CanStack || (ItemInfo.CanStack && ItemInfo.Amount == 1))
        {
            DevideButton->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void UInventoryActionMenuWidget::UseButtonClicked()
{
    if(InventoryComp)
    {
        InventoryComp->UseItemAtIndex(Index);
        SetVisibility(ESlateVisibility::Hidden);
    }
}

void UInventoryActionMenuWidget::CancelButtonClicked()
{
    SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryActionMenuWidget::DevideButtonClicked()
{
    if(InventoryComp)
    {
        InventoryComp->DevideStack(Index, ItemInfo.Amount / 2);
        SetVisibility(ESlateVisibility::Hidden);
    }
}

void UInventoryActionMenuWidget::ThrowButtonClicked()
{
    if(InventoryComp)
    {
        if(ItemInfo.Amount == 1)
        {
            InventoryComp->RemoveItemAtSlot(Index, ItemInfo.Amount);
            SetVisibility(ESlateVisibility::Hidden);
            return;
        }
    }
    ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwningPlayerPawn());
    if(OwnerCharacter)
    {
        UMainHUDWidget* MainHUD = OwnerCharacter->GetMainHUDWidget();
        if(MainHUD)
        {
            UItemThrowWidget* ItemThrowWidget = MainHUD->GetItemThrowWidget();
            if(ItemThrowWidget)
            {
                ItemThrowWidget->Init(ItemInfo, Index);
                ItemThrowWidget->Update();
                SetVisibility(ESlateVisibility::Hidden);
            }
            UInventoryWidget* InventoryWidget = MainHUD->GetInventoryWidget();
            if(InventoryWidget && InventoryWidget->GetSlotWidgets()[Index])
            {
                InventoryWidget->GenerateSlotWidgets();
                InventoryWidget->GetSlotWidgets()[Index]->SetIsEnabled(false);
            }
        }
    }
}