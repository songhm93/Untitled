#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interface/CombatInterface.h"
#include "../Interface/TargetingInterface.h"
#include "../Interface/EquipmentInterface.h"
#include "../Type/Types.h"
#include "../Type/DamageTypes.h"
#include "MeleeCharacter.generated.h"


class UCombatComponent;
class UStateManagerComponent;
class UTargetingComponent;
class UStatsComponent;
class UPrimitiveComponent;
class UWidgetComponent;
class UDamageType;
class AController;
class USoundCue;
class UParticleSystem;
class ABaseEquippable;


UCLASS(config=Game)
class AMeleeCharacter : public ACharacter, public ICombatInterface, public ITargetingInterface, public IEquipmentInterface
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
	virtual bool CanBeTargeted() override; //타겟이 될 수 있는지
	virtual void OnTargeted(bool IsTargeted) override;
	virtual float PerformCombatAction(ECurrentAction Action, ECurrentState State) override;
	UFUNCTION(BlueprintCallable)
	virtual void PerformLightAttack(int32 AttackCount) override;

	virtual void Equip(ABaseEquippable* Weapon) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
	void ToggleCombat();
	void InteractButtonPressed();
	void PerformAttack(ECurrentAction Action);
	
	bool CanAttack();
	bool CanToggleCombat();
	void Dodge();
	void PerformDodge();
	bool CanDodge();
	void PerformAction(int32 MontageIdx, TArray<UAnimMontage*> Montage, ECurrentState State = ECurrentState::GENERAL_STATE, ECurrentAction Action = ECurrentAction::GENERAL_ACTION);
	void ToggleWalk();
	void SprintButtonPressed();
	void SprintButtonReleased();
	void HeavyAttack();
	
private:
	void TurnRight(float Rate);
	void LookUp(float Rate);
	void Test(); //테스트할 함수
	UPROPERTY(EditAnywhere, Category = "MouseSensitivity", Meta = (AllowPrivateAccess = "true"))
	float MouseSensitivity;
	UFUNCTION()
	void ReceiveDamage(
		AActor* DamagedActor, 
		float EnemyATK, 
		AController* InstigatedBy, 
		FVector HitLocation, 
		UPrimitiveComponent* FHitComponent, 
		FName BoneName, 
		FVector ShotFromDirection, 
		const UDamageType* DamageType, 
		AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Comp", Meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComp;
	

	// Common
	UPROPERTY(EditDefaultsOnly, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactFrontMontage;
	UPROPERTY(EditDefaultsOnly, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactBackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KnockdownFrontMontage;
	UPROPERTY(EditDefaultsOnly, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KnockdownBackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DodgeMontage; 
	UPROPERTY(EditDefaultsOnly, Category = "Common", Meta = (AllowPrivateAccess = "true"))
	USoundCue* ImpactSound;
	UPROPERTY(EditDefaultsOnly, Category = "Common", Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle;

	//Light Sword
	UPROPERTY(EditDefaultsOnly, Category = "LSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LSLightAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "LSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LSChargedAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "LSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LSHeavyAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "LSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LSSprintAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "LSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LSEnterCombatMontage;
	UPROPERTY(EditDefaultsOnly, Category = "LSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* LSExitCombatMontage;

	//Great Sword
	UPROPERTY(EditDefaultsOnly, Category = "GSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* GSLightAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "GSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* GSChargedAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "GSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* GSHeavyAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "GSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* GSEnterCombatMontage;
	UPROPERTY(EditDefaultsOnly, Category = "GSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* GSExitCombatMontage;

	//Dual Sword
	UPROPERTY(EditDefaultsOnly, Category = "DSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DSLightAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "DSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DSChargedAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "DSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DSHeavyAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "DSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DSSprintAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "DSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DSEnterCombatMontage;
	UPROPERTY(EditDefaultsOnly, Category = "DSMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DSExitCombatMontage;
	

	/*****************************************************/

	void Dead();
	void CalcReceiveDamage(float Damage);
	void EnableRagdoll();
	FName PelvisBoneName;
	void ApplyHitReactionPhysicsVelocity(float InitSpeed);

	FTimerHandle DestroyDeadTimerHandle; //죽은 캐릭터, 무기 destory
	float DestroyDeadTime;
	void DestroyDead();
	
	UPROPERTY(VisibleAnywhere)
	UStateManagerComponent* StateManagerComp;

	UFUNCTION()
	void CharacterStateBegin(ECurrentState State);
	UFUNCTION()
	void CharacterStateEnd(ECurrentState State);
	UFUNCTION()
	void CharacterActionBegin(ECurrentAction Action);
	UFUNCTION()
	void CharacterActionEnd(ECurrentAction Action);

	UPROPERTY(VisibleAnywhere)
	EMovementType CurrentMovementType;
	float WalkSpeed;
	float JogSpeed;
	float SprintSpeed;
	bool bHeavyAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UStatsComponent* StatComp;
	float DodgeStaminaCost;
	float SprintStaminaCost;
	bool bSprintKeyPressed;

	void ToggleLockOn();
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	UTargetingComponent* TargetingComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* LockOnWidget;
	void ApplyHitReaction(EDamageType DamageType);
	void PerformHitReact();
	void PerformKnockdown();
	bool bHitFront; //맞았을 때 때린 캐릭터가 내 앞에서 때렸는지?
	void ApplyImpactEffect(EDamageType DamageType, FVector HitLocation);
	FName GetLightAttackSectionName(int32 AttackCount);
public: //get
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UCombatComponent* GetCombatComp() const { return CombatComp; }
	FORCEINLINE UAnimMontage* GetDodgeMontage() const { return DodgeMontage; }
	FORCEINLINE UStateManagerComponent* GetStateManagerComp() const { return StateManagerComp;}
	FORCEINLINE EMovementType GetMovementType() const { return CurrentMovementType; }
	FORCEINLINE float GetSprintStaminaCost() const { return SprintStaminaCost; }
	FORCEINLINE UStatsComponent* GetStatComp() const { return StatComp;}
public: //set
	void SetMovementType(EMovementType Type);
public:
	void LightAttack();
	void ChargedAttack();
	
	
};

