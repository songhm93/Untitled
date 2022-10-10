#pragma once


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    LIGHT_SWORD UMETA(DisplayName="LIGHT_SWORD"),
    GREAT_SWORD UMETA(DisplayName="GREAT_SWORD"),

    MAX UMETA(DisplayName="MAX")
};

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    WEAPON UMETA(DisplayName="WEAPON"),
    ARMOR UMETA(DisplayName="ARMOR"),

    MAX UMETA(DisplayName="MAX")
};

UENUM(BlueprintType)
enum class EArmorType : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    HELMET UMETA(DisplayName="HELMET"),
    CHEST UMETA(DisplayName="CHEST"),
    GAUNTLET UMETA(DisplayName="GAUNTLET"),
    BOOT UMETA(DisplayName="BOOT"),

    MAX UMETA(DisplayName="MAX")
};

UENUM(BlueprintType)
enum class ECharacterState : uint8 //캐릭터의 상태
{
    NOTHING UMETA(DisplayName = "NOTHING"),
    ATTACKING UMETA(DisplayName = "ATTACKING"),
    DODGING UMETA(DisplayName = "DODGING"),
    GENERAL_STATE UMETA(DisplayName = "GENERAL_STATE"),
    DEAD UMETA(DisplayName = "DEAD"),
    DISABLED UMETA(DisplayName = "DISABLED"),

    MAX UMETA(DisplayName = "MAX")
};

UENUM(BlueprintType)
enum class ECharacterAction : uint8 //애니메이션 상태
{
    NOTHING UMETA(DisplayName = "NOTHING"),
    GENERAL_ACTION UMETA(DisplayName = "GENERAL_ACTION"),
    LIGHT_ATTACK UMETA(DisplayName = "LIGHT_ATTACK"),
    HEAVY_ATTACK UMETA(DisplayName = "HEAVY_ATTACK"),
    CHARGED_ATTACK UMETA(DisplayName = "CHARGED_ATTACK"),
    SPRINT_ATTACK UMETA(DisplayName = "SPRINT_ATTACK"),
    DODGE UMETA(DisplayName = "DODGE"),
    ENTER_COMBAT UMETA(DisplayName = "ENTER_COMBAT"),
    EXIT_COMBAT UMETA(DisplayName = "EXIT_COMBAT"),

    MAX UMETA(DisplayName = "MAX")
};

UENUM(BlueprintType)
enum class EDataTableType : uint8
{
    COMMON_TABLE UMETA(DisplayName = "COMMON"),
    LIGHT_SWORD_TABLE UMETA(DisplayName = "LIGHT_SWORD_TABLE"),
    GREAT_SWORD_TABLE UMETA(DisplayName = "GREAT_SWORD_TABLE"),
    DUAL_SWORD_TABLE UMETA(DisplayName = "DUAL_SWORD_TABLE"),

    MAX UMETA(DisplayName = "MAX")
};

UENUM(BlueprintType)
enum class EMovementType : uint8
{
    WALKING UMETA(DisplayName = "WALKING"),
    JOGGING UMETA(DisplayName = "JOGGING"),
    SPRINTING UMETA(DisplayName = "SPRINGTING"),

    MAX UMETA(DisplayName = "MAX")
};



