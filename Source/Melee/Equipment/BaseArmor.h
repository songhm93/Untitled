#pragma once

#include "CoreMinimal.h"
#include "BaseEquippable.h"
#include "BaseArmor.generated.h"


UCLASS()
class MELEE_API ABaseArmor : public ABaseEquippable
{
	GENERATED_BODY()

public:
	ABaseArmor(){}
	ABaseArmor(float _ArmorDef, EArmorType _ArmorType);

private:
	UPROPERTY(EditAnywhere, Category = "Stat")
	float ArmorDEF;

	UPROPERTY(EditAnywhere, Category = "Type")
	EArmorType ArmorType;

public:
	FORCEINLINE EArmorType GetArmorType() const { return ArmorType; }
	FORCEINLINE float GetArmorDEF() const { return ArmorDEF; }

public:
	FORCEINLINE void SetArmorType(EArmorType Type) { ArmorType = Type; }
	FORCEINLINE void SetArmorDEF(float DEF) { ArmorDEF = DEF; }
};
