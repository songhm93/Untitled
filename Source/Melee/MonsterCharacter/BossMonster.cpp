#include "BossMonster.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

#include "../Melee.h"
#include "../Component/MonsterStatsComponent.h"
#include "../Widget/EnemyHPBarWidget.h"
#include "../Component/StateManagerComponent.h"


ABossMonster::ABossMonster()
{
    GetMesh()->SetCollisionObjectType(MonsterCapsule);

    GetMonsterStatComp()->SetSpecialATK(FSpecialATK(3.f, 4.f, 5.f));

    HPBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidgetComp"));

    HideHPBarTime = 3.f;

    bExecuteHide = false;

    HideTime = 0.f;
}

void ABossMonster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(bExecuteHide)
    {
        HideTime += DeltaTime;
        if(HideTime >= 1.f)
        {
            HiddenHPBar();
        }
    }
    
}

void ABossMonster::BeginPlay()
{
    Super::BeginPlay();

    if(StateManagerComp)
	{
        StateManagerComp->OnCombatState.AddUObject(this, &ThisClass::HPBarOnOff);
    }

    if(HPBarWidgetComp)
	{
		BossHPBarWidget = CreateWidget<UEnemyHPBarWidget>(GetWorld(), HPBarWidgetComp->GetWidgetClass());
		if(BossHPBarWidget)
		{
            BossHPBarWidget->AddToViewport();
            BossHPBarWidget->SetVisibility(ESlateVisibility::Hidden);
            //BossHPBarWidget->AddToViewport();
            //HPBarWidgetComp->SetWidget(BossHPBarWidget);
            //HPBarWidgetComp->SetVisibility(false);
            Cast<UEnemyHPBarWidget>(BossHPBarWidget)->Init(MonsterStatComp);
            Cast<UEnemyHPBarWidget>(BossHPBarWidget)->GetBossNameText()->SetText(BossName);
            
		}
	}

}

void ABossMonster::Special1()
{
}

void ABossMonster::Special2()
{
}

void ABossMonster::Special3()
{
}

void ABossMonster::OnTargeted(bool IsTargeted)
{
    if(BossHPBarWidget)
	{
		if(IsTargeted)
        {
            BossHPBarWidget->SetVisibility(ESlateVisibility::Visible);
            Cast<UEnemyHPBarWidget>(BossHPBarWidget)->PlayOpacityAnim(true);
        }
        else
        {
            Cast<UEnemyHPBarWidget>(BossHPBarWidget)->PlayOpacityAnim(false);
            bExecuteHide = true;
            //BossHPBarWidget->SetVisibility(ESlateVisibility::Hidden);
        }	
	}
}

void ABossMonster::HideHPBar()
{
	if(BossHPBarWidget)
	{
		Cast<UEnemyHPBarWidget>(BossHPBarWidget)->PlayOpacityAnim(false);
        bExecuteHide = true;
        //BossHPBarWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABossMonster::HiddenHPBar()
{
    if(BossHPBarWidget)
	{
        BossHPBarWidget->SetVisibility(ESlateVisibility::Hidden);
    }
    bExecuteHide = false;
    HideTime = 0.f;

}

void ABossMonster::HPBarOnOff(bool Show)
{
	if(BossHPBarWidget)
	{
		if(Show)
		{
			BossHPBarWidget->SetVisibility(ESlateVisibility::Visible);
            Cast<UEnemyHPBarWidget>(BossHPBarWidget)->PlayOpacityAnim(true);
		}
		else
		{
			GetWorldTimerManager().SetTimer(HideHPBarTimerHandle, this, &ThisClass::HideHPBar, HideHPBarTime);
		}
	}
}

void ABossMonster::Dead()
{
	Super::Dead();

    if(BossHPBarWidget)
    {
        Cast<UEnemyHPBarWidget>(BossHPBarWidget)->PlayOpacityAnim(false);
        bExecuteHide = true;
    }
}


