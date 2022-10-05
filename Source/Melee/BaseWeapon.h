#pragma once

#include "CoreMinimal.h"
#include "BaseEquippable.h"
#include "Types.h"
#include "BaseWeapon.generated.h"

class UAnimMontage;

UCLASS()
class MELEE_API ABaseWeapon : public ABaseEquippable
{
	GENERATED_BODY()
	
public:
	ABaseWeapon();
	virtual void OnEquipped() override; 
protected:
	
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EnterCombatMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ExitCombatMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Init", Meta = (AllowPrivateAccess = "true"))
	FName HandSocketName;
	ECombatType CombatType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim", Meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> AttackMontage; //공격 애니메이션 몽타주를 배열에 넣어서 관리. 콤보공격을 위함.
public: //get
	FORCEINLINE UAnimMontage* GetEnterCombatAM() const { return EnterCombatMontage; }
	FORCEINLINE UAnimMontage* GetExitCombatAM() const { return ExitCombatMontage; }
	FORCEINLINE FName GetHandSocketName() const { return HandSocketName; }
	FORCEINLINE ECombatType GetCombatType() const { return CombatType; }
	FORCEINLINE TArray<UAnimMontage*> GetAttackMontage() const { return AttackMontage; }
public: //set
	FORCEINLINE void SetEnterCombatAM(UAnimMontage* AM) { EnterCombatMontage = AM;}
	FORCEINLINE void SetExitCombatAM(UAnimMontage* AM) { ExitCombatMontage = AM;}
	FORCEINLINE void SetHandSocketName(FName SocketName) { HandSocketName = SocketName;}
	FORCEINLINE void SetCombatType(ECombatType Type) { CombatType = Type;}
	
public:
	UFUNCTION(BlueprintCallable)
	void AttachWeapon(AMeleeCharacter* Character);
	void SetAttackMontage(UAnimMontage* Montage);
};	

