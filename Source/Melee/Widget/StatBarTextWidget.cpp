#include "StatBarTextWidget.h"
#include "../Component/StatsComponent.h"
#include "Components/TextBlock.h"

void UStatBarTextWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    StatComp = Cast<UStatsComponent>(GetOwningPlayerPawn()->GetComponentByClass(UStatsComponent::StaticClass()));
    if(StatComp)
    {
        StatComp->OnPlusCurrentStatValue.AddDynamic(this, &ThisClass::OnStatValueUpdated);
    }
}

void UStatBarTextWidget::OnStatValueUpdated(EStats Stat, float Value)
{
    if((StatType == Stat) && StatComp && StatNumber)
    {
        FString StatText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Value), FMath::CeilToInt(StatComp->GetMaxValue(Stat)));
        StatNumber->SetText(FText::FromString(StatText));
    }
}