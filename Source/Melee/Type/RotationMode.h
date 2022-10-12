#pragma once


UENUM(BlueprintType)
enum class ERotationMode : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    ORIENT_TO_CAMERA UMETA(DisplayName="ORIENT_TO_CAMERA"),
    ORIENT_TO_MOVEMENT UMETA(DisplayName = "ORIENT_TO_MOVEMENT"), 

    MAX UMETA(DisplayName="MAX")
};