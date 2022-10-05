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

};	

