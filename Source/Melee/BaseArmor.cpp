#include "BaseArmor.h"

#include "Type/Types.h"

ABaseArmor::ABaseArmor()
{
    SetEquipmentType(EEquipmentType::ARMOR);
    ArmorDEF = 20.f;
}