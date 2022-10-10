#pragma once

#include "CoreMinimal.h"
#include "BaseEquippable.h"
#include "BaseArmor.generated.h"


UCLASS()
class MELEE_API ABaseArmor : public ABaseEquippable
{
	GENERATED_BODY()

public:
	ABaseArmor();
private:
	UPROPERTY(EditAnywhere, Category = "Stat")
	float ArmorDEF;
	UPROPERTY(EditAnywhere, Category = "Type")
	EArmorType ArmorType;
public:
	EArmorType GetArmorType() const { return ArmorType; }
	float GetArmorDEF() const { return ArmorDEF; }
};
