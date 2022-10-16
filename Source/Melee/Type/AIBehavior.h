#pragma once


UENUM(BlueprintType)
enum class EAIBehavior : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    ATTACK UMETA(DisplayName="ATTACK"),
    CHASE UMETA(DisplayName="CHASE"),
    PATROL UMETA(DisplayName="PATROL"),
    HIT UMETA(DisplayName="HIT"),

    MAX UMETA(DisplayName="MAX")
};