#pragma once


UENUM(BlueprintType)
enum class EDamageType : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    MELEE_DAMAGE UMETA(DisplayName="MELEE_DAMAGE"),
    KNOCKDOWN_DAMAGE UMETA(DisplayName="KNOCKDOWN_DAMAGE"),
    
    MAX UMETA(DisplayName="MAX")
};