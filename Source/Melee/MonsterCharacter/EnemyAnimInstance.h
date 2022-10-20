#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

class AEnemyCharacter;

UCLASS()
class MELEE_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	
private:
	UFUNCTION()
	void AnimNotify_ResetCombat();

	void UpdateCombatState(bool CombatState);

	UPROPERTY()
	AEnemyCharacter* EnemyCharacter;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	bool bCombatState;
	
};
