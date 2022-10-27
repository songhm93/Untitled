#pragma once

#include "CoreMinimal.h"
#include "BaseEquippable.h"
#include "../Type/Types.h"
#include "../Type/Elements.h"
#include "BaseWeapon.generated.h"

class UAnimMontage;

UCLASS()
class MELEE_API ABaseWeapon : public ABaseEquippable
{
	GENERATED_BODY()
	
public:
	ABaseWeapon();
	virtual void Skill1();
	virtual void Skill2();
	virtual void Skill3();
	virtual void SkillUltimate();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Comp", Meta = (AllowPrivateAccess = "true"))
	float WeaponATK; //무기 공격력
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Skill1Montage;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Skill2Montage;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Skill3Montage;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SkillUltimateMontage;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	float Skill1Cooldown;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	float Skill2Cooldown;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	float Skill3Cooldown;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	float SkillUltimateCooldown;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Init", Meta = (AllowPrivateAccess = "true"))
	FName HandSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = "true"))
	EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere, Category = "Element", Meta = (AllowPrivateAccess = "true"))
	EElements CurrentElement;

public: //get
	FORCEINLINE FName GetHandSocketName() const { return HandSocketName; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE float GetWeaponATK() const { return WeaponATK; }
	FORCEINLINE float GetSkill1Cooldown() const { return Skill1Cooldown; }
	FORCEINLINE float GetSkill2Cooldown() const { return Skill2Cooldown; }
	FORCEINLINE float GetSkill3Cooldown() const { return Skill3Cooldown; }
	FORCEINLINE float GetSkillUltimateCooldown() const { return SkillUltimateCooldown; }
public: //set
	FORCEINLINE void SetHandSocketName(FName SocketName) { HandSocketName = SocketName;}
	FORCEINLINE void SetWeaponType(EWeaponType Type) { WeaponType = Type;}
	
public:
	void SimulateWeaponPhysics();
	
	
};	

