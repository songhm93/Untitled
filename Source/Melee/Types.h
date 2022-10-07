#pragma once


UENUM(BlueprintType)
enum class ECombatType : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    LIGHT_SWORD UMETA(DisplayName="LIGHT_SWORD"),
    GREAT_SWORD UMETA(DisplayName="GREAT_SWORD"),

    MAX UMETA(DisplayName="MAX")
};

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    NOTHING UMETA(DisplayName = "NOTHING"),
    ATTACKING UMETA(DisplayName = "ATTACKING"),
    DODGING UMETA(DisplayName = "DODGING"),
    GENERAL_ACTION_STATE UMETA(DisplayName = "GENERAL_ACTION_STATE"),
    DEAD UMETA(DisplayName = "DEAD"),
    DISABLED UMETA(DisplayName = "DISABLED"),

    MAX UMETA(DisplayName = "MAX")
};