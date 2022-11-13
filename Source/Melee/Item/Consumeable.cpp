#include "Consumeable.h"

#include "../PlayerCharacter/BaseCharacter.h"
#include "../Component/StatsComponent.h"
#include "../Component/StateManagerComponent.h"


AConsumeable::AConsumeable()
{
    
}

void AConsumeable::Tick(float DeltaTime)
{

}

void AConsumeable::UseItem(int32 ItemId, int32 SlotIndex)
{
    Super::UseItem(ItemId, SlotIndex);

    bool Success = false;
    //ItemId에 따라 기능 구분
    if(ItemId == 100001) //HP
    {
        Success = UsePotion(true);
    }
    else if(ItemId == 100002) //스태미나
    {
        Success = UsePotion(false);
    }

    UseSuccess.ExecuteIfBound(Success, SlotIndex);
    Destroy();
}

bool AConsumeable::UsePotion(bool IsHPPotion) //true면 HP포션 false면 스태미나포션.
{
    ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
    if(OwnerCharacter && OwnerCharacter->GetStatComp() && OwnerCharacter->GetStateManagerComp())
    {
        if(IsHPPotion && !OwnerCharacter->GetStateManagerComp()->GetIsHPPotionCooldown())
        {
            if(OwnerCharacter->GetStatComp()->GetMaxValue(EStats::HP) == OwnerCharacter->GetStatComp()->GetCurrentStatValue(EStats::HP))
                return false;
            float MaxHP = OwnerCharacter->GetStatComp()->GetMaxValue(EStats::HP);
            OwnerCharacter->GetStatComp()->PlusCurrentStatValue(EStats::HP, MaxHP * 0.3f);
            OwnerCharacter->GetStateManagerComp()->SetPotionCooldown(IsHPPotion);
            return true;
        }
        else if(!IsHPPotion && !OwnerCharacter->GetStateManagerComp()->GetIsStaminaPotionCooldown())
        {
            if(OwnerCharacter->GetStatComp()->GetMaxValue(EStats::STAMINA) == OwnerCharacter->GetStatComp()->GetCurrentStatValue(EStats::STAMINA))
                return false;
            float MaxStamina = OwnerCharacter->GetStatComp()->GetMaxValue(EStats::STAMINA);
            OwnerCharacter->GetStatComp()->PlusCurrentStatValue(EStats::STAMINA, MaxStamina * 0.3f);
            OwnerCharacter->GetStateManagerComp()->SetPotionCooldown(IsHPPotion);
            return true;
        }
    }


    return false;
}