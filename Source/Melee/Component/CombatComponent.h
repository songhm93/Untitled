#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Type/Types.h"
#include "../Type/Stats.h"
#include "CombatComponent.generated.h"

DECLARE_DELEGATE_TwoParams(FOnUpdateCurrentStatValue, EStats, float);
DECLARE_DELEGATE_RetVal_OneParam(float, FGetCurrentStatValue, EStats);
DECLARE_DELEGATE_OneParam(FOnUpdateWeaponType, EWeaponType);
DECLARE_DELEGATE_RetVal(ECurrentCombatState, FGetCurrentCombatState);

class ABaseWeapon;
class ABaseEquippable;
class ABaseArmor;
class UAttackDamageType;

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
protected:
	virtual void BeginPlay() override;
private:	
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
	FVector HitFromDirection;

	UPROPERTY()
	UAttackDamageType* AttackDamageType;

	void WeaponBaseSetting();
	void ArmorBaseSetting(ABaseArmor* Armor);
	float AttackActionCorrectionValue;
public: //get
	FORCEINLINE ABaseWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool GetIsAttackSaved() const { return bIsAttackSaved; }
	FORCEINLINE int32 GetAttackCount() const { return AttackCount; }
public: //set
	FORCEINLINE void SetIsAttackSaved(bool Boolean) { bIsAttackSaved = Boolean; }
	FORCEINLINE void SetAttackCount(int32 Count) { AttackCount = Count; }
	FORCEINLINE void SetAttackActionCorrectionValue(float Value) { AttackActionCorrectionValue = Value; }
public:
	FORCEINLINE void IncrementAttackCount() { ++AttackCount; }
	FORCEINLINE void ResetAttackCount() { AttackCount = 0; }

	void OnEquipped(ABaseEquippable* Equipment);
	void AttachActor(EEquipmentType Type, FName SocketName);
	void AttachWeapon();
	void AttachSecondWeapon(FName SocketName);

	void HitCauseDamage(FHitResult& HitResult);

};
