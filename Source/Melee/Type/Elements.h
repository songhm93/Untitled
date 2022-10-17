#pragma once

UENUM(BlueprintType)
enum class EElements : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    WATER UMETA(DisplayName = "WATER"),
    FIRE UMETA(DisplayName = "FIRE"),
    EARTH UMETA(DisplayName = "EARTH"),

    MAX UMETA(DisplayName = "MAX")
};