#pragma once

#include "CoreMinimal.h"
#include "BaseEquippable.h"
#include "../Type/Types.h"
#include "../Type/Elements.h"
#include "Http.h"
#include "Engine/DataTable.h"
#include "BaseWeapon.generated.h"


class UAnimMontage;

USTRUCT(BlueprintType) //db
struct FPlayerSkillInfo
{
	GENERATED_BODY()
	UPROPERTY()
	int32 Sid = 0;
	UPROPERTY()
	int32 Pid = 0;
	UPROPERTY()
	int32 Skillnum = 0;
	UPROPERTY()
	int32 Skilllevel = 0;
	UPROPERTY()
	int32 Weaponid = 0;
	
};

USTRUCT(BlueprintType)
struct FSkillInfo
{
	GENERATED_BODY()

	FSkillInfo(){}
	FSkillInfo(UTexture2D* _Texture, FText _SkillName, int32 _NeedToLevel)
	{
		SkillIcon = _Texture;
		SkillName = _SkillName;
		NeedToLevel = _NeedToLevel;
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* SkillIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NeedToLevel;
	
};

USTRUCT(BlueprintType)
struct FSkillInfoTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* SkillIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NeedToLevel;
	
};

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
	UFUNCTION(BlueprintCallable)
	virtual void InitSkillInfo();
	virtual void InitSkillDT();
protected:
	FHttpModule* Http;

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

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	float Skill1ATK;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	float Skill2ATK;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	float Skill3ATK;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	float Skill4ATK;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	int32 WeaponId;

	void OnSkillInfoRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool Success);

	TArray<FPlayerSkillInfo> ConvertToPlayerSkillInfo(const FString& ResponseString);

	void GetSkillInfoDB();

	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = "true"))
	TMap<int32, FSkillInfo> SkillInfo;

	float GetPlayerATK();

	
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
	UFUNCTION(BlueprintCallable)
	FORCEINLINE TMap<int32, FSkillInfo> GetSkillInfo() const { return SkillInfo; }
public: //set
	FORCEINLINE void SetHandSocketName(FName SocketName) { HandSocketName = SocketName;}
	FORCEINLINE void SetWeaponType(EWeaponType Type) { WeaponType = Type;}
public:
	int32 GetWeaponSkillATK(int32 SkillNum);
	int32 SkillATKCalc(int32 SkillNum);
	
	
};	

