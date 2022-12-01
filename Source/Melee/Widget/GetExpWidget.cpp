#include "GetExpWidget.h"
#include "Components/TextBlock.h"

#include "../PlayerCharacter/BaseCharacter.h"
#include "MainHUDWidget.h"


void UGetExpWidget::NativeConstruct()
{
    Super::NativeConstruct();

    Player = Cast<ABaseCharacter>(GetOwningPlayerPawn());

    RemoveWidgetTime = 1.f;
}

void UGetExpWidget::FadeAnim(int32 ExpValue)
{
    if(Fade && ExpPoint)
    {
        ExpPoint->SetText(FText::FromString(FString::FromInt(ExpValue)));
        PlayAnimation(Fade);
        GetWorld()->GetTimerManager().SetTimer(RemoveWidgetTimerHandle, this, &ThisClass::RemoveWidget, RemoveWidgetTime);
    }
}

void UGetExpWidget::RemoveWidget()
{
    if(Player && Player->GetMainHUDWidget())
    {
        Player->GetMainHUDWidget()->RemoveGetExpWidget();
    }
}