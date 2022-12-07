#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "DualWeapon.generated.h"

DECLARE_DELEGATE_RetVal(bool, FGetSkillTimerRunning);
DECLARE_DELEGATE_TwoParams(FSetSkillATK, float, FHitResult);

class UCurveFloat;
class UParticleSystem;
class UParticleSystemComponent;
class AFlameSkill;
class UUserWidget;
class UTexture2D;







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
	FSetSkillATK SetSkillATK;
	virtual void InitSkillInfo() override;
	virtual void InitSkillDT() override;
protected:
	

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

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BlinkSupplementalParticle;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* BlinkSupplementalParticleComponent;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BlinkBeginParticle;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* BlinkBeginParticleComponent;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BlinkReturnParticle;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* BlinkShadowBurstParticle;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* Skill1ExplodeParticle;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* Skill2FlameParticle;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AFlameSkill> FlameActor;

	FTransform ShadowTransform;

	bool bPlayCurve;

	FTimerHandle HeightCurveTimerHandle;

	float CurveTime;

	FVector CurrentLocation; //궁극기 사용시 캐릭터의 위치

	FVector BlinkCurrentLocation; //블링크 사용시 캐릭터의 위치

	float BlinkReturnCooldown;

	FTimerHandle BlinkReturnTimerHandle;

	bool bBlinkReturnTimerRunning;

	bool bReturnComplete;
	
	FTimerHandle Skill1ExpodeTimerHandle;

	float Skill1ExpodeTime;

	FTimerHandle BlinkShowCharacterTimerHandle;

	FTimerDelegate BlinkShowCharacterDelegate;

	float BlinkShowCharacterTime;

	FTimerHandle BlinkMoveTimerHandle;

	float BlinkMoveTime;

	void SetPlayerHeight();

	void UltimateSocketChange(bool Start);

	void UltimateComplete();

	void FinishSetHeight();

	void Skill1Explode();

	void Skill2Range();

	void BlinkMove();
	
	void BlinkDestroyShadow();

	int32 Skill1ATKCalc();

public: //get
	FORCEINLINE UStaticMeshComponent* GetDualSwordStaticMeshComp() const { return DualSwordStaticMeshComp;}
	FORCEINLINE FName GetSecondWeaponHandSocket() const { return SecondWeaponHandSocket; }
	FORCEINLINE FName GetSecondWeaponAttachSocket() const { return SecondWeaponAttachSocket; }
	FORCEINLINE bool GetbBlinkReturnTimerRunning() const { return bBlinkReturnTimerRunning; }
	
public:
	UFUNCTION()
	void ShowHideCharacter(bool Hide);

};
