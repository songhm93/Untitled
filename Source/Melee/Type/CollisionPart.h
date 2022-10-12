#pragma once


UENUM(BlueprintType)
enum class ECollisionPart : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    MAIN_WEAPON UMETA(DisplayName="MAIN_WEAPON"),
    SECOND_WEAPON UMETA(DisplayName="SECOND_WEAPON"),
    RIGHT_FOOT UMETA(DisplayName="RIGHT_FOOT"),

    MAX UMETA(DisplayName="MAX")
};