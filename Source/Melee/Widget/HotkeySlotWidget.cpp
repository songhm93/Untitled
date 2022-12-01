#include "HotkeySlotWidget.h"
#include "Components/Image.h"

#include "../PlayerCharacter/BaseCharacter.h"

void UHotkeySlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if(CooldownMaterialInst && Cooldown)
    {
        CooldownDynamicMaterialInst = UMaterialInstanceDynamic::Create(CooldownMaterialInst, this);

        Cooldown->SetBrushFromMaterial(CooldownDynamicMaterialInst);
    }

    MeleeCharacter = Cast<ABaseCharacter>(GetOwningPlayerPawn());
}