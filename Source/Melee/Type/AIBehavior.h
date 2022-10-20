#pragma once


UENUM(BlueprintType)
enum class EAIBehavior : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    ATTACK UMETA(DisplayName="ATTACK"),
    CHASE UMETA(DisplayName="CHASE"),
    PATROL UMETA(DisplayName="PATROL"),
    RETURN UMETA(DisplayName="RETURN"),

    MAX UMETA(DisplayName="MAX")
};