#pragma once

#include "CoreMinimal.h"
#include "BaseEquippable.h"
#include "Types.h"
#include "BaseWeapon.generated.h"

class UAnimMontage;
class UCollisionComponent;

UCLASS()
class MELEE_API ABaseWeapon : public ABaseEquippable
{
	GENERATED_BODY()
	
public:
	ABaseWeapon();
	virtual void OnEquipped() override; 
protected:
	
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Anim", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EnterCombatMontage;
	UPROPERTY(VisibleAnywhere, Category = "Anim", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ExitCombatMontage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Init", Meta = (AllowPrivateAccess = "true"))
	FName HandSocketName;
	ECombatType CombatType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Comp", Meta = (AllowPrivateAccess = "true"))
	UCollisionComponent* CollisionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Comp", Meta = (AllowPrivateAccess = "true"))
	float Damage;
public: //get
	FORCEINLINE UAnimMontage* GetEnterCombatAM() const { return EnterCombatMontage; }
	FORCEINLINE UAnimMontage* GetExitCombatAM() const { return ExitCombatMontage; }
	FORCEINLINE FName GetHandSocketName() const { return HandSocketName; }
	FORCEINLINE ECombatType GetCombatType() const { return CombatType; }
	
public: //set
	FORCEINLINE void SetEnterCombatAM(UAnimMontage* AM) { EnterCombatMontage = AM;}
	FORCEINLINE void SetExitCombatAM(UAnimMontage* AM) { ExitCombatMontage = AM;}
	FORCEINLINE void SetHandSocketName(FName SocketName) { HandSocketName = SocketName;}
	FORCEINLINE void SetCombatType(ECombatType Type) { CombatType = Type;}
	
public:
	UFUNCTION(BlueprintCallable)
	void AttachWeapon(AMeleeCharacter* Character);
	void OnHit(FHitResult& HitResult);
};	

