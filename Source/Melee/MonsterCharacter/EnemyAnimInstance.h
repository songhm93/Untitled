#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

DECLARE_DELEGATE(FOnDeattachRock);
DECLARE_DELEGATE(FOnEnemyImpact);

class AEnemyCharacter;

UCLASS()
class MELEE_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	FOnDeattachRock OnDeattachRock;
	FOnEnemyImpact OnEnemyImpact;

private:
	UFUNCTION()
	void AnimNotify_ResetCombat();

	UFUNCTION()
	void AnimNotify_SpecialComplete();

	UFUNCTION()
	void AnimNotify_DeattachRock();

	UFUNCTION()
	void AnimNotify_Impact();

	UFUNCTION()
	void AnimNotify_DefaultCameraShake();

	void UpdateCombatState(bool CombatState);

	void UpdateSpecialReady();

	UPROPERTY()
	AEnemyCharacter* EnemyCharacter;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	bool bCombatState;

	FTimerHandle SpecialReadyTimerHandle;

	float SpecialReadyTime;

public:
	FORCEINLINE void SetSpecialReadyTime(float Time) { SpecialReadyTime = Time; }
	
};
