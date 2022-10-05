#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Types.h"
#include "MeleeAnimInstance.generated.h"

class AMeleeCharacter;

UCLASS()
class MELEE_API UMeleeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	float Speed;
	UPROPERTY()
	AMeleeCharacter* Character;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	ECombatType CombatType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	bool bCombatEnabled;
	
public:
	FORCEINLINE void SetCombatType(ECombatType Type) { CombatType = Type; }
	FORCEINLINE void SetCombatEnabled(bool Boolean) { bCombatEnabled = Boolean; }
};
