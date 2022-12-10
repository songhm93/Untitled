#include "EnemyHPBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "../Component/StatsComponent.h"
#include "../MonsterCharacter/EnemyCharacter.h"


void UEnemyHPBarWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    
}

void UEnemyHPBarWidget::NativeConstruct()
{
    Super::NativeConstruct();


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
    if(StatComp)
    {  
        StatComp->OnPlusCurrentStatValue.AddDynamic(this, &ThisClass::OnStatValueUpdated);
        StatComp->OnPlusCurrentStatValue.Broadcast(EStats::HP, StatComp->GetMaxValue(EStats::HP));
    }
}

void UEnemyHPBarWidget::PlayOpacityAnim(bool Forward)
{
    if(OpacityAnim)
    {
        if(Forward)
            PlayAnimation(OpacityAnim);
        else
            PlayAnimationReverse(OpacityAnim);
    }   
    
}

void UEnemyHPBarWidget::SetMonsterName(FString Name)
{
    if(MonsterName)
    {
        MonsterName->SetText(FText::FromString(Name));
    }
}
