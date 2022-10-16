#include "StatBarWidget.h"
#include "Components/ProgressBar.h"
#include "../Component/StatsComponent.h"


void UStatBarWidget::NativePreConstruct()
{
    
}

void UStatBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    StatComp = Cast<UStatsComponent>(GetOwningPlayerPawn()->GetComponentByClass(UStatsComponent::StaticClass()));
    if(StatComp)
    {
        StatComp->OnPlusCurrentStatValue.AddDynamic(this, &ThisClass::OnStatValueUpdated);
    }
}

void UStatBarWidget::OnStatValueUpdated(EStats Stat, float Value)
{
    if((StatType == Stat) && StatComp && StatBar)
    {
        float ResultValue = Value / StatComp->GetMaxValue(Stat);
        StatBar->SetPercent(ResultValue);
    }
}