#pragma once


UENUM(BlueprintType)
enum class EStats : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    HP UMETA(DisplayName="HP"),
    ATK UMETA(DisplayName = "ATK"), 
    DEF UMETA(DisplayName="DEF"),
    SP UMETA(DisplayName="SP"),

    MAX UMETA(DisplayName="MAX")
};
//스탯의 종류

UENUM(BlueprintType)
enum class EExpStats : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    LEVEL UMETA(DisplayName="LEVEL"),
    CURRENT_EXP UMETA(DisplayName = "CURRENT_EXP"), 
    MAX_EXP UMETA(DisplayName="MAX_EXP"),

    MAX UMETA(DisplayName="MAX")
};