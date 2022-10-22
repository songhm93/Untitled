#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

DECLARE_DELEGATE(FOnDeattachRock);

class AEnemyCharacter;

UCLASS()
class MELEE_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	FOnDeattachRock OnDeattachRock;
private:
	UFUNCTION()
	void AnimNotify_ResetCombat();

	UFUNCTION()
	void AnimNotify_SpecialComplete();

	UFUNCTION()
	void AnimNotify_DeattachRock();

	void UpdateCombatState(bool CombatState);

	void UpdateSpecialReady();

	UPROPERTY()
	AEnemyCharacter* EnemyCharacter;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	bool bCombatState;

	FTimerHandle SpecialReadyTimerHandle;

	float SpecialReadyTime;
	
};
