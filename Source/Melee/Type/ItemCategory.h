#pragma once

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
    
	NONE UMETA(DisplayName = "NONE"),
    CONSUMEABLE UMETA(DisplayName="CONSUMEABLE"),
    WEAPON UMETA(DisplayName="WEAPON"),
    HELMET UMETA(DisplayName="HELMET"),
    CHEST UMETA(DisplayName="CHEST"),
    GAUNTLET UMETA(DisplayName="GAUNTLET"),
    BOOT UMETA(DisplayName="BOOT"),
    QUEST UMETA(DisplayName="QUEST"),
    READABLES UMETA(DisplayName="READABLES"),
    GOLD UMETA(DisplayName="GOLD"),

    MAX UMETA(DisplayName="MAX")
};