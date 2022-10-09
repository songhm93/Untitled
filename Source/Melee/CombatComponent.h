#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class ABaseWeapon;
class AMeleeCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;
private:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta=(AllowPrivateAccess = "true"))
	ABaseWeapon* EquippedWeapon;
	UPROPERTY(VisibleAnywhere, Meta=(AllowPrivateAccess = "true"))
	bool bCombatState;
	UPROPERTY(VisibleAnywhere, Meta=(AllowPrivateAccess = "true"))
	int32 AttackCount;
	UPROPERTY(VisibleAnywhere, Meta=(AllowPrivateAccess = "true"))
	bool bIsAttackSaved;
	UPROPERTY()
	AController* Controller;
	FVector HitFromDirection;
	
public: //get
	FORCEINLINE ABaseWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool GetCombatState() const { return bCombatState; }
	FORCEINLINE bool GetIsAttackSaved() const { return bIsAttackSaved; }
	FORCEINLINE int32 GetAttackCount() const { return AttackCount; }
public: //set
	FORCEINLINE void SetCombatState(bool Boolean) { bCombatState = Boolean; }
	FORCEINLINE void SetIsAttackSaved(bool Boolean) { bIsAttackSaved = Boolean; }
	FORCEINLINE void SetAttackCount(int32 Count) { AttackCount = Count; }
public:
	FORCEINLINE void IncrementAttackCount() { ++AttackCount; }
	FORCEINLINE void ResetAttackCount() { AttackCount = 0; }

	void OnEquipped(ABaseWeapon* Weapon);
	void AttachActor(FName SocketName);
	void AttachWeapon(AMeleeCharacter* Character);
	void OnUnequipped();
	void HitCauseDamage(FHitResult& HitResult, ABaseWeapon* Weapon);

};
