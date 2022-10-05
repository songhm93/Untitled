#pragma once


UENUM(BlueprintType)
enum class ECombatType : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    LIGHT_SWORD UMETA(DisplayName="LIGHT_SWORD"),
    GREAT_SWORD UMETA(DisplayName="GREAT_SWORD"),

    MAX UMETA(DisplayName="MAX")
};