#pragma once


UENUM(BlueprintType)
enum class EAIBasicBehavior : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    BASIC_ATTACK UMETA(DisplayName="ATTACK"),
    CHASE UMETA(DisplayName="CHASE"),
    PATROL UMETA(DisplayName="PATROL"),
    RETURN UMETA(DisplayName="RETURN"),

    MAX UMETA(DisplayName="MAX")
};

UENUM(BlueprintType)
enum class EBossBehavior : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    BASIC_ATTACK UMETA(DisplayName="ATTACK"),
    CHASE UMETA(DisplayName="CHASE"),
    PATROL UMETA(DisplayName="PATROL"),
    SPECIAL1 UMETA(DisplayName="SPECIAL1"),
    SPECIAL2 UMETA(DisplayName="SPECIAL2"),
    SPECIAL3 UMETA(DisplayName="SPECIAL3"),
    RETURN UMETA(DisplayName="RETURN"),

    MAX UMETA(DisplayName="MAX")
};

