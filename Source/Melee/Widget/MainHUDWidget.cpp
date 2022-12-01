#include "MainHUDWidget.h"

#include "GetExpWidget.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"


void UMainHUDWidget::NativeConstruct()
{
    Super::NativeConstruct();

    ExistGetExpWidgetNum = 0;
    WaitGetExpWidgetNum = 0;
}

void UMainHUDWidget::GetExp(int32 Exp)
{
    if(ExistGetExpWidgetNum == 4) 
    {
        GetExpWidgetQueue.Add(Exp);
        ++WaitGetExpWidgetNum;
        return;
    }
	if(GetExpWidgetClass)
	{
		GetExpWidget = CreateWidget<UGetExpWidget>(GetWorld(), GetExpWidgetClass);
		if(GetExpWidget && GetExpBox)
		{
			++ExistGetExpWidgetNum;
            GetExpBox->AddChild(GetExpWidget);
            //GetExpWidget->AddToViewport();
			GetExpWidget->FadeAnim(Exp);

            GetExpWidgetScrollUp();
            
		}
	}
}

void UMainHUDWidget::GetExpWidgetScrollUp()
{
    for(UGetExpWidget* WidgetLog : GetExpWidgetList)
    {
        if(WidgetLog)
        {
            UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(WidgetLog->GetExpBox);
            if(CanvasSlot)
            {
                FVector2D Position = CanvasSlot->GetPosition();
                FVector2D NewPosition(CanvasSlot->GetPosition().X, Position.Y - CanvasSlot->GetSize().Y);
                CanvasSlot->SetPosition(NewPosition);
            }
        }
    }
    GetExpWidgetList.Add(GetExpWidget);
}

void UMainHUDWidget::RemoveGetExpWidget()
{
    GetExpWidget = *(GetExpWidgetList.begin());
    GetExpWidget->RemoveFromParent();
    GetExpWidgetList.RemoveAt(0);
    --ExistGetExpWidgetNum;
    if(WaitGetExpWidgetNum > 0) //화면에 떠있던 위젯들 지워질 때 떠야할 대기중인 위젯이 있으면
    {
        GetExp(GetExpWidgetQueue[0]);
        GetExpWidgetQueue.RemoveAt(0);
        --WaitGetExpWidgetNum;
    }
}