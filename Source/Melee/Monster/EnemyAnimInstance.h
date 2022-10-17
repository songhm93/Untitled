#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"


//DECLARE_DELEGATE(FOnApplyDamage);

UCLASS()
class MELEE_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	//FOnApplyDamage OnApplyDamage;
	

private:

	/*UFUNCTION()
	void AnimNotify_ApplyDamage();*/
};
