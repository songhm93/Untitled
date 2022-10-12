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
//스탯의 종류