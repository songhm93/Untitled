#include "PMMob.h"
#include "Components/WidgetComponent.h"

#include "../Component/StateManagerComponent.h"
#include "../Widget/EnemyHPBarWidget.h"
#include "../Component/MonsterStatsComponent.h"

APMMob::APMMob()
{
    HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
    HPBarWidget->SetupAttachment(GetMesh());

    
	HideHPBarTime = 3.f;
}

void APMMob::BeginPlay()
{
    Super::BeginPlay();

    if(StateManagerComp)
	{
        StateManagerComp->OnCombatState.AddUObject(this, &ThisClass::HPBarOnOff);
    }
	
	if(HPBarWidget)
	{
		UUserWidget* EnemyHPBarWidget = CreateWidget<UEnemyHPBarWidget>(GetWorld(), HPBarWidget->GetWidgetClass());
		if(EnemyHPBarWidget)
		{
			HPBarWidget->SetWidget(EnemyHPBarWidget);
			HPBarWidget->SetVisibility(false);
			Cast<UEnemyHPBarWidget>(EnemyHPBarWidget)->Init(MonsterStatComp);
		}
	}
}

void APMMob::OnTargeted(bool IsTargeted)
{
	Super::OnTargeted(IsTargeted);

    if(HPBarWidget)
	{
		if(IsTargeted)
			HPBarWidget->SetVisibility(IsTargeted);
	}
}

void APMMob::HideHPBar()
{
	if(HPBarWidget)
	{
		HPBarWidget->SetVisibility(false);
	}
}

void APMMob::HPBarOnOff(bool Show)
{
	if(HPBarWidget)
	{
		if(Show)
		{
			if(GetWorldTimerManager().IsTimerActive(HideHPBarTimerHandle))
				GetWorldTimerManager().ClearTimer(HideHPBarTimerHandle);
			HPBarWidget->SetVisibility(Show);
		}
		else
		{
			GetWorldTimerManager().SetTimer(HideHPBarTimerHandle, this, &ThisClass::HideHPBar, HideHPBarTime);
		}
	}
}

void APMMob::Dead()
{
	Super::Dead();

	if(HPBarWidget)
		HPBarWidget->SetVisibility(false);
}