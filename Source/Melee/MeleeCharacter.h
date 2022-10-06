// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatInterface.h"
#include "Engine/DataTable.h"
#include "MeleeCharacter.generated.h"


class AToughSword;
class ABaseWeapon;
class UCombatComponent;
class UDamageType;
class AController;
class UPrimitiveComponent;
class USoundCue;
class UParticleSystem;

USTRUCT(BlueprintType)
struct FResourceTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> LightAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> GreatAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* DodgeMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* ImpactSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* ImpactParticle;
};

UCLASS(config=Game)
class AMeleeCharacter : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AMeleeCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;
	virtual void Jump() override;
	virtual void ContinueAttack() override;
	virtual void ResetAttack() override;
	UFUNCTION(BlueprintCallable)
	virtual FRotator GetDesiredRotation() override;
	virtual void ResetCombat() override;
	virtual bool CanRecieveDamage() override;
protected:
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
	void ToggleCombat();
	void PerformAttack(int32 AttackIdx, bool bRandomIdx);
	void LightAttack();
	bool CanAttack();
	bool CanToggleCombat();
	void Dodge();
	void PerformDodge(int32 MontageIdx, bool bRandomIdx);
	bool CanDodge();
	
protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	void Test(); //테스트할 함수

	void InteractButtonPressed();
	UFUNCTION()
	void ReceiveDamage(
		AActor* DamagedActor, 
		float Damage, 
		AController* InstigatedBy, 
		FVector HitLocation, 
		UPrimitiveComponent* FHitComponent, 
		FName BoneName, 
		FVector ShotFromDirection, 
		const UDamageType* DamageType, 
		AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Comp", Meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComp;

	bool bTogglingCombat;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	bool bDodging;

	UPROPERTY(VisibleAnywhere, Category = "Anim", Meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> LightAttackMontage; //공격 애니메이션 몽타주를 배열에 넣어서 관리. 콤보공격을 위함. 무기 타입마다 몽타주 추가 해주기.
	UPROPERTY(VisibleAnywhere, Category = "Anim", Meta = (AllowPrivateAccess = "true"))
	TArray<UAnimMontage*> GreatAttackMontage;
	UPROPERTY(VisibleAnywhere, Category = "Anim", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DodgeMontage; //회피 애니메이션 몽타주
	UAnimMontage* HitReactMontage;
	USoundCue* ImpactSound;
	UParticleSystem* ImpactParticle;
	
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	bool bIsDisabled; //맞았을 때 아무것도 못하게
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	float HP;
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	bool bIsDead;
	void Dead();
	void CauseDamage(float Damage);
	void EnableRagdoll();
	FName PelvisBoneName;
	void ApplyHitReactionPhysicsVelocity(float InitSpeed);

	FTimerHandle DestroyDeadTimerHandle; //죽은 캐릭터, 무기 destory
	float DestroyDeadTime;
	void DestroyDead();
public: //get
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UCombatComponent* GetCombatComp() const { return CombatComp; }
	FORCEINLINE TArray<UAnimMontage*> GetLightAttackMontage() const { return LightAttackMontage; }
	FORCEINLINE TArray<UAnimMontage*> GetGreatAttackMontage() const { return GreatAttackMontage; }
	FORCEINLINE UAnimMontage* GetDodgeMontage() const { return DodgeMontage; }
public: //set
	FORCEINLINE void SetDodging(bool Boolean) { bDodging = Boolean; }
	FORCEINLINE void SetTogglingCombat(bool Boolean) { bTogglingCombat = Boolean; }
public:
	void SetLightAttackMontage(UAnimMontage* Montage);
	void SetGreatAttackMontage(UAnimMontage* Montage);
	void SetDodgeMontage(UAnimMontage* Montage);
	
};

