#pragma once

UENUM(BlueprintType)
enum class ENPCType : uint8
{
    NOTHING UMETA(DisplayName = "NOTHING"),
    MERCHANT UMETA(DisplayName = "MERCHANT"),
    BUFF UMETA(DisplayName = "BUFF"),

    MAX UMETA(DisplayName = "MAX")
};