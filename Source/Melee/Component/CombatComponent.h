#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Type/Types.h"
#include "../Type/Stats.h"
#include "CombatComponent.generated.h"

DECLARE_DELEGATE_TwoParams(FOnUpdateCurrentStatValue, EStats, float);
DECLARE_DELEGATE_OneParam(FOnUpdateWeaponType, EWeaponType);
DECLARE_DELEGATE_OneParam(FOnUpdateCurrentState, ECurrentState);
DECLARE_DELEGATE_OneParam(FOnUpdateCurrentAction, ECurrentAction);
DECLARE_DELEGATE_RetVal_OneParam(float, FGetCurrentStatValue, EStats);
DECLARE_DELEGATE_RetVal_OneParam(bool, FIsCurrentStateEqualToThis, TArray<ECurrentState>);
DECLARE_DELEGATE_RetVal(ECurrentCombatState, FGetCurrentCombatState);
DECLARE_DELEGATE_RetVal(ECurrentState, FGetCurrentState);
DECLARE_DELEGATE_RetVal(EMovementType, FGetCurrentMovementType);


class ABaseWeapon;
class ABaseEquippable;
class ABaseArmor;
class UAttackDamageType;
class UStateManagerComponent;
class UAnimMontage;
class USoundCue;
class UParticleSystem;
class UAnimInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FOnUpdateCurrentStatValue OnUpdateCurrentStatValue;
	FGetCurrentStatValue GetCurrentStatValue;
	FOnUpdateWeaponType OnUpdateWeaponType;
	FGetCurrentCombatState GetCurrentCombatState;
	FGetCurrentState GetCurrentState;
	FIsCurrentStateEqualToThis IsCurrentStateEqualToThis;
	FOnUpdateCurrentState OnUpdateCurrentState;
	FOnUpdateCurrentAction OnUpdateCurrentAction;
	FGetCurrentMovementType GetCurrentMovementType;
	
protected:
	virtual void BeginPlay() override;
	virtual FName GetLightAttackSectionName(int32 AttackCount);
	virtual void LightAttack();
	FVector HitFromDirection;
	UPROPERTY()
	UAnimInstance* AnimInst;
	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BasicAttackImpactParticle;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	USoundCue* BasicAttackImpactSound;
	virtual void ImpactTrace();
	void ApplyImpact(AActor* HitActor);
	void ClearHitActors();
	
private:	
	void WeaponBaseSetting();

	void ArmorBaseSetting(ABaseArmor* Armor);

	void Attack(int32 AttackCount);

	void ChargedAttack();

	void SubAttack(ECurrentAction Action);

	bool CanAttack();

	void ApplySkillExplodeDamage(float SkillATK, FHitResult HitResult);

	void UltimateImpact();

	// Common
	UPROPERTY(EditDefaultsOnly, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DodgeMontage;

	//Light Sword
	UPROPERTY(EditDefaultsOnly, Category = "LSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LSLightAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "LSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LSChargedAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "LSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LSHeavyAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "LSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LSSprintAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "LSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LSEnterCombatMontage;

	UPROPERTY(EditDefaultsOnly, Category = "LSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LSExitCombatMontage;

	//Dual Sword
	UPROPERTY(EditDefaultsOnly, Category = "DSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DSLightAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "DSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DSChargedAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "DSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DSHeavyAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "DSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DSSprintAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "DSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DSEnterCombatMontage;

	UPROPERTY(EditDefaultsOnly, Category = "DSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DSExitCombatMontage;
	
	/*****************************************************/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta=(AllowPrivateAccess = "true"))
	ABaseWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta=(AllowPrivateAccess = "true"))
	ABaseArmor* EquippedHelmet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta=(AllowPrivateAccess = "true"))
	ABaseArmor* EquippedGauntlet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta=(AllowPrivateAccess = "true"))
	ABaseArmor* EquippedChest;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta=(AllowPrivateAccess = "true"))
	ABaseArmor* EquippedBoot;

	UPROPERTY(VisibleAnywhere, Meta=(AllowPrivateAccess = "true"))
	int32 AttackCount;
	
	UPROPERTY(VisibleAnywhere, Meta=(AllowPrivateAccess = "true"))
	bool bIsAttackSaved;

	UPROPERTY()
	AController* Controller;

	UPROPERTY()
	UAttackDamageType* AttackDamageType;
	
	float DodgeStaminaCost;

	float AttackActionCorrectionValue;

	float FirstSkillSlotCooldown;

	float SecondSkillSlotCooldown;

	float ThirdSkillSlotCooldown;

	float UltimateSkillSlotCooldown;

	FTimerHandle FirstSkillTimerHandle;

	FTimerHandle SecondSkillTimerHandle;

	FTimerHandle ThirdSkillTimerHandle;

	FTimerHandle UltimateSkillTimerHandle;

	bool bFirstSkillTimerRunning;

	bool bSecondSkillTimerRunning;

	bool bThirdSkillTimerRunning;

	bool bUltimateSkillTimerRunning;

public: //get
	FORCEINLINE ABaseWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool GetIsAttackSaved() const { return bIsAttackSaved; }
	FORCEINLINE int32 GetAttackCount() const { return AttackCount; }
	FORCEINLINE UAnimMontage* GetLSEnterCombatMontage() const { return LSEnterCombatMontage; }
	FORCEINLINE UAnimMontage* GetLSExitCombatMontage() const { return LSExitCombatMontage; }
	FORCEINLINE UAnimMontage* GetDSEnterCombatMontage() const { return DSEnterCombatMontage; }
	FORCEINLINE UAnimMontage* GetDSExitCombatMontage() const { return DSExitCombatMontage; }
	FORCEINLINE float GetDodgeStaminaCost() const { return DodgeStaminaCost; }
	UFUNCTION()
	FORCEINLINE bool GetThirdSkillTimerRunning() const { return bThirdSkillTimerRunning; }
	
public: //set
	FORCEINLINE void SetIsAttackSaved(bool Boolean) { bIsAttackSaved = Boolean; }
	FORCEINLINE void SetAttackCount(int32 Count) { AttackCount = Count; }
	FORCEINLINE void SetAttackActionCorrectionValue(float Value) { AttackActionCorrectionValue = Value; }

public:
	FORCEINLINE void IncrementAttackCount() { ++AttackCount; }
	FORCEINLINE void ResetAttackCount() { AttackCount = 0; }
	void OnEquipWeapon(ABaseWeapon* Equipment);
	void OnEquipArmor(ABaseArmor* Equipment);
	void EquippedWeaponSpawn(ABaseWeapon* Equipment, int32 WeaponId);
	void OnEquippedArmorApply(ABaseArmor* Equipment);
	void AttachActor(EEquipmentType Type, FName SocketName);
	void AttachWeapon();
	void AttachSecondWeapon(FName SocketName);
	void HitCauseDamage(FHitResult& HitResult);
	void PerformDodge();
	void ContinueAttack();
	void HeavyAttack();
	UFUNCTION(BlueprintCallable)
	void Skill1();
	UFUNCTION(BlueprintCallable)
	void Skill2();
	UFUNCTION(BlueprintCallable)
	void Skill3();
	UFUNCTION(BlueprintCallable)
	void SkillUltimate();
	UFUNCTION(BlueprintCallable)
	float GetSkillCurrentCooldownTime(int32 SkillNum);
	UFUNCTION(BlueprintCallable)
	float GetWeaponSkillCooldownTime(int32 SkillNum);
	
};
