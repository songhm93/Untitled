#include "EnemyAnimInstance.h"


void UEnemyAnimInstance::NativeInitializeAnimation()
{
   
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{

}

void UEnemyAnimInstance::AnimNotify_ApplyDamage()
{
    OnApplyDamage.ExecuteIfBound();
}