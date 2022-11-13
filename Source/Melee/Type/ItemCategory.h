#pragma once

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
    
	NONE UMETA(DisplayName = "NONE"),
    CONSUMEABLE UMETA(DisplayName="CONSUMEABLE"),
    EQUIPMENT UMETA(DisplayName="EQUIPMENT"),
    QUEST UMETA(DisplayName="QUEST"),
    READABLES UMETA(DisplayName="READABLES"),

    MAX UMETA(DisplayName="MAX")
};