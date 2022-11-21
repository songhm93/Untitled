#include "ItemThrowWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"

#include "../Component/InventoryComponent.h"
#include "../PlayerCharacter/BaseCharacter.h"
#include "MainHUDWidget.h"
#include "InventoryWidget.h"

void UItemThrowWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if(ThrowButton)
    {
        ThrowButton->OnClicked.AddDynamic(this, &ThisClass::ThrowButtonClicked);
    }
    if(CancelButton)
    {
        CancelButton->OnClicked.AddDynamic(this, &ThisClass::CancelButtonClicked);
    }
    if(InputTextBox)
    {
        InputTextBox->OnTextChanged.AddDynamic(this, &ThisClass::ReceiveThrowNum);
    }

    ThrowNum = 0;

    if(GetOwningPlayerPawn())
        InventoryComp = Cast<UInventoryComponent>(GetOwningPlayerPawn()->GetComponentByClass(UInventoryComponent::StaticClass()));
}

void UItemThrowWidget::Init(FItemInfoInSlot ItemInfoInSlot, int32 SlotIndex)
{
    ItemInfo = ItemInfoInSlot;
    Index = SlotIndex;
    Update();
}

void UItemThrowWidget::Update() //위젯을 띄우고 정보 업데이트
{
    SetVisibility(ESlateVisibility::Visible);
    if(InputTextBox)
    {
        InputTextBox->SetText(FText::FromString(TEXT("")));
        InputTextBox->SetHintText(FText::FromString(TEXT("갯수를 입력하세요.")));
    }

    if(ItemIcon && NameText && NumText && InputTextBox)
    {
        ItemIcon->SetBrushFromTexture(ItemInfo.Icon);
        NameText->SetText(FText::FromString(ItemInfo.ItemName));
        NumText->SetText(FText::FromString(FString::FromInt(ItemInfo.Amount)));
    }
}

void UItemThrowWidget::CancelButtonClicked()
{
    SetVisibility(ESlateVisibility::Hidden);
    ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwningPlayerPawn());
    if(OwnerCharacter)
    {
        UMainHUDWidget* MainHUD = OwnerCharacter->GetMainHUDWidget();
        if(MainHUD)
        {
            UInventoryWidget* InventoryWidget = MainHUD->GetInventoryWidget();
            if(InventoryWidget && InventoryWidget->GetSlotWidgets()[Index])
            {
                InventoryWidget->GetSlotWidgets()[Index]->SetIsEnabled(true);
            }
        }
    }
}

void UItemThrowWidget::ReceiveThrowNum(const FText& Text)
{
    FString ThrowNumString = Text.ToString();
    
    if(ThrowNumString.IsNumeric())
    {
        ThrowNum = FCString::Atof(*ThrowNumString);
    } 
}

void UItemThrowWidget::ThrowButtonClicked()
{
    if(InventoryComp)
    {
        InventoryComp->RemoveItemAtSlot(Index, ThrowNum);
        SetVisibility(ESlateVisibility::Hidden);
    }
}