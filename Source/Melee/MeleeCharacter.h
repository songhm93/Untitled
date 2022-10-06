// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatInterface.h"
#include "MeleeCharacter.generated.h"

class AToughSword;
class ABaseWeapon;
class UCombatComponent;

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
	virtual void ContinueAttack() override;
	virtual void ResetAttack() override;
	UFUNCTION(BlueprintCallable)
	virtual FRotator GetDesiredRotation() override;
	virtual void ResetCombat() override;
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
	void InteractButtonPressed();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComp;
	bool bTogglingCombat;
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	bool bDodging;
public: //get
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UCombatComponent* GetCombatComp() const { return CombatComp; }
public: //set
	FORCEINLINE void SetDodging(bool Boolean) { bDodging = Boolean; }
public:
};

