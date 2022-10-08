#pragma once


UENUM(BlueprintType)
enum class EStats : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    HP UMETA(DisplayName="HP"),
    STAMINA UMETA(DisplayName="STAMINA"),
    ARMOR UMETA(DisplayName="ARMOR"),

    MAX UMETA(DisplayName="MAX")
};