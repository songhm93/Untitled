#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../Type/Types.h"
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	float Speed;
	UPROPERTY()
	AMeleeCharacter* Character;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	bool bCombatState;
	UFUNCTION()
	void AnimNotify_Equip();
	UFUNCTION()
	void AnimNotify_UnEquip();
	UFUNCTION()
	void AnimNotify_ContinueAttack();
	UFUNCTION()
	void AnimNotify_ResetCombat();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	float Direction;
public:
	UFUNCTION()
	void SetWeaponType(EWeaponType Type);
	FORCEINLINE void SetCombatState(bool Boolean) { bCombatState = Boolean; }
};
