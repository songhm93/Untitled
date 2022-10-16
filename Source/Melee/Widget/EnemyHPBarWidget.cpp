#include "EnemyHPBarWidget.h"
#include "Components/ProgressBar.h"
#include "../Component/StatsComponent.h"
#include "../EnemyCharacter.h"


void UEnemyHPBarWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    
}

void UEnemyHPBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    //UStatsComponent* Temp =  Cast<UStatsComponent>(GetOwningPlayer()->GetComponentByClass(UStatsComponent::StaticClass()));
    //Init(Temp);

}

void UEnemyHPBarWidget::OnStatValueUpdated(EStats Stat, float Value)
{
    if((StatType == Stat) && StatComp && EnemyHPBar)
    {
        float ResultValue = Value / StatComp->GetMaxValue(Stat);
        EnemyHPBar->SetPercent(ResultValue);
    }
}

void UEnemyHPBarWidget::Init(UStatsComponent* Comp)
{
    StatComp = Comp;
    if(StatComp)//AI의 StatComp가 아니라.. 나의 StatComp가 되는데..
    {  
        StatComp->OnPlusCurrentStatValue.AddDynamic(this, &ThisClass::OnStatValueUpdated);
        StatComp->OnPlusCurrentStatValue.Broadcast(EStats::HP, StatComp->GetMaxValue(EStats::HP));
    }
}
