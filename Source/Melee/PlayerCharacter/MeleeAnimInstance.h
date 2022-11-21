#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "../Type/Types.h"
#include "MeleeAnimInstance.generated.h"

DECLARE_DELEGATE(FOnImpact);
DECLARE_DELEGATE(FOnUltimateComplete);
DECLARE_DELEGATE(FOnUltimateImpact);

class ABaseCharacter;

UCLASS()
class MELEE_API UMeleeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	FOnImpact OnImpact;
	FOnUltimateComplete OnUltimateComplete;
	FOnUltimateImpact OnUltimateImpact;
private:
	
	UFUNCTION()
	void AnimNotify_Equip();

	UFUNCTION()
	void AnimNotify_UnEquip();

	UFUNCTION()
	void AnimNotify_ContinueAttack();

	UFUNCTION()
	void AnimNotify_ResetCombat();

	UFUNCTION()
	void AnimNotify_DodgeComplete();

	UFUNCTION()
	void AnimNotify_Impact();

	UFUNCTION()
	void AnimNotify_UltimateComplete();

	UFUNCTION()
	void AnimNotify_UltimateImpact();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY()
	ABaseCharacter* Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	bool bCombatState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	float Direction;

public: //set
	FORCEINLINE void SetCombatState(bool Boolean) { bCombatState = Boolean; }
	
public:
	UFUNCTION()
	void SetWeaponType(EWeaponType Type);
	
};
