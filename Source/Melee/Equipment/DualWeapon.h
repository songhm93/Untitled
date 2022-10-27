#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "DualWeapon.generated.h"


DECLARE_DELEGATE_RetVal(bool, FGetSkillTimerRunning);

class UCurveFloat;
class UParticleSystem;
class UParticleSystemComponent;

UCLASS()
class MELEE_API ADualWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	ADualWeapon();
	virtual void Skill1() override;
	virtual void Skill2() override;
	virtual void Skill3() override;
	virtual void SkillUltimate() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	FGetSkillTimerRunning GetSkillTimerRunning;
private:
	UPROPERTY(VisibleAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* DualSwordStaticMeshComp;

	UPROPERTY(EditAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	FName SecondWeaponAttachSocket;

	UPROPERTY(EditAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	FName SecondWeaponHandSocket;

	UPROPERTY(EditAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	FName FirstUltimateSocket;

	UPROPERTY(EditAnywhere, Category = "Mesh", Meta = (AllowPrivateAccess = "true"))
	FName SecondUltimateSocket;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UCurveFloat* PlayerHeightCurve;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BlinkShadowParticle;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* BlinkShadowParticleComponent;

	bool bPlayCurve;

	void SetPlayerHeight(float DeltaTime);

	FTimerHandle HeightCurveTimerHandle;

	float CurveTime;

	void UltimateSocketChange(bool Start);

	void UltimateComplete();

	void FinishSetHeight();

	FVector CurrentLocation; //궁극기 사용시 캐릭터의 위치

	FVector BlinkCurrentLocation; //블링크 사용시 캐릭터의 위치

	float BlinkReturnCooldown;

	FTimerHandle BlinkReturnTimerHandle;

	bool bBlinkReturnTimerRunning;

	bool bReturnComplete;
	
public: //get
	FORCEINLINE UStaticMeshComponent* GetDualSwordStaticMeshComp() const { return DualSwordStaticMeshComp;}
	FORCEINLINE FName GetSecondWeaponHandSocket() const { return SecondWeaponHandSocket; }
	FORCEINLINE FName GetSecondWeaponAttachSocket() const { return SecondWeaponAttachSocket; }
	FORCEINLINE bool GetbBlinkReturnTimerRunning() const { return bBlinkReturnTimerRunning; }
	
public:
};
