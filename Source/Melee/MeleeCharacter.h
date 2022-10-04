// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MeleeCharacter.generated.h"

class AToughSword;
class ABaseWeapon;

UCLASS(config=Game)
class AMeleeCharacter : public ACharacter
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

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void Interact(AActor* Caller);
private:
	UPROPERTY(EditAnywhere, Category = "Mesh", Meta=(AllowPrivateAccess = "true"))
	TSubclassOf<AActor> SpawnActor;
	UPROPERTY(BlueprintReadWrite, Category = "Combat", Meta=(AllowPrivateAccess = "true"))
	ABaseWeapon* EquippedWeapon;
	UPROPERTY(EditAnywhere, Category = "Combat" , Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontage;
	UPROPERTY(EditAnywhere, Category = "Combat" , Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* UnequipMontage;
	void InteractButtonPressed();
public: //get
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
public: //set
	FORCEINLINE void SetWeapon(ABaseWeapon* Weapon) { EquippedWeapon = Weapon; }
};

