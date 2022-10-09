#pragma once


UENUM(BlueprintType)
enum class EStats : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    HP UMETA(DisplayName="HP"),
    ATK UMETA(DisplayName = "ATK"), 
    DEF UMETA(DisplayName="DEF"),
    STAMINA UMETA(DisplayName="STAMINA"),

    MAX UMETA(DisplayName="MAX")
};
//플레이어 스탯, 장비 스탯 등 다 합친 수치