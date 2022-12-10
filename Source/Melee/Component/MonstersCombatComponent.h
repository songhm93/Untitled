#pragma once

#include "CoreMinimal.h"
#include "CombatComponent.h"
#include "MonstersCombatComponent.generated.h"

DECLARE_DELEGATE_RetVal_OneParam(float, FGetCurrentStatValue, EStats);

class AEnemyAIController;
class ACharacter;
class UParticleSystem;
class USoundCue;

UCLASS()
class MELEE_API UMonstersCombatComponent : public UCombatComponent
{
	GENERATED_BODY()

public:	
	UMonstersCombatComponent();
	FGetCurrentStatValue GetCurrentStatValue;
	UFUNCTION(BlueprintCallable)
	virtual void LightAttack() override;

protected:
	virtual void BeginPlay() override;
	virtual FName GetLightAttackSectionName(int32 AttackCount) override;
	
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void MonsterImpactTrace();

	FName GetAttackSectionName();

	void ReadyToAttack();

	UPROPERTY(EditAnywhere, Category= "AttackMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* CloseRangeAttackMontage;

	UPROPERTY()
	AEnemyAIController* AIController;

	UPROPERTY()
	ACharacter* OwnerCharacter;


	bool bCanAttack;

	bool bEnemyWeaponCollisionEnabled;

	FName RightWeaponStartSocketName;

	FName RightWeaponEndSocketName;

	FName LeftWeaponStartSocketName;

	FName LeftWeaponEndSocketName;

	FName WeaponStartSocketName;

	FName WeaponEndSocketName;

	float CloseAttackCorrectionValue;

	FTimerHandle ReadyToAttackTimerHandle;

	float ReadyToAttackTime;
	
public:	//get
	FORCEINLINE	bool GetIsCollisionEnabled() const { return bEnemyWeaponCollisionEnabled; }
	FORCEINLINE	bool CanAttack() const { return bCanAttack; }
public: //set
	FORCEINLINE void SetStartSocketName(FName SocketName) { RightWeaponStartSocketName = SocketName; }
	FORCEINLINE void SetEndSocketName(FName SocketName) { RightWeaponEndSocketName = SocketName; }
	FORCEINLINE void SetCloseAttackCorrectionValue(float Value) { CloseAttackCorrectionValue = Value; }

public:
};
