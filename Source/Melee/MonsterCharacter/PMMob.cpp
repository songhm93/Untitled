#include "PMMob.h"
#include "Components/WidgetComponent.h"

#include "../Component/StateManagerComponent.h"
#include "../Widget/EnemyHPBarWidget.h"
#include "../Component/MonsterStatsComponent.h"

APMMob::APMMob()
{

}

void APMMob::BeginPlay()
{
    Super::BeginPlay();
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

void APMMob::Dead()
{
	Super::Dead();

	if(HPBarWidget)
		HPBarWidget->SetVisibility(false);
}