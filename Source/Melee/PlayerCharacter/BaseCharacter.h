#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interface/CombatInterface.h"
#include "../Interface/TargetingInterface.h"
#include "../Interface/EquipmentInterface.h"
#include "../Type/Types.h"
#include "../Type/DamageTypes.h"
#include "BaseCharacter.generated.h"

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
class USpringArmComponent;
class UCameraComponent;

UCLASS(config=Game)
class ABaseCharacter : public ACharacter, public ICombatInterface, public ITargetingInterface, public IEquipmentInterface
{
	GENERATED_BODY()
public:
	ABaseCharacter();
	virtual void Jump() override;
	UFUNCTION(BlueprintCallable)
	virtual void ResetCombat() override;
	virtual bool CanRecieveDamage() override;
	virtual bool CanBeTargeted() override; //타겟이 될 수 있는지
	virtual void OnTargeted(bool IsTargeted) override;
	virtual void Equip(ABaseEquippable* Weapon) override;
	virtual void CalcReceiveDamage(float ATK) override;
	virtual void ApplyHitReaction(EDamageType DamageType) override;
	virtual void ApplyImpactEffect(EDamageType DamageType, FVector HitLocation) override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void CameraZoomInOut(float Rate);
	
	void ToggleCombat();

	void InteractButtonPressed();
	
	bool CanToggleCombat();

	void Dodge();

	bool CanDodge();

	void PerformAction(UAnimMontage* Montage, ECurrentState State = ECurrentState::GENERAL_STATE, ECurrentAction Action = ECurrentAction::GENERAL_ACTION);

	void ToggleWalk();

	void SprintButtonPressed();

	void SprintButtonReleased();
	
private:
	void Test(); //테스트할 함수

	void TurnRight(float Rate);

	void LookUp(float Rate);

	void Dead();

	void EnableRagdoll();

	void ApplyHitReactionPhysicsVelocity(float InitSpeed);

	void DestroyDead();

	void ToggleLockOn();

	void PerformHitReact();

	void PerformKnockdown();

	FName GetLightAttackSectionName(int32 AttackCount);

	void Skill1ButtonPressed();

	void Skill2ButtonPressed();

	void Skill3ButtonPressed();

	void SkillUltimateButtonPressed();

	UFUNCTION(BlueprintCallable)
	FRotator GetDesiredRotation();

	UFUNCTION()
	void CharacterStateBegin(ECurrentState State);

	UFUNCTION()
	void ReceiveDamage(
		AActor* DamagedActor, 
		float EnemyATK, 
		const UDamageType* DamageType, 
		AController* InstigatedBy, 
		AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", Meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatCompo;

	UPROPERTY(VisibleAnywhere)
	UStateManagerComponent* StateManagerComp;

	UPROPERTY(EditAnywhere, Category = "MouseSensitivity", Meta = (AllowPrivateAccess = "true"))
	float MouseSensitivity;

	UPROPERTY(EditDefaultsOnly, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactFrontMontage;

	UPROPERTY(EditDefaultsOnly, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactBackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KnockdownFrontMontage;

	UPROPERTY(EditDefaultsOnly, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KnockdownBackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Common", Meta = (AllowPrivateAccess = "true"))
	USoundCue* ImpactSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Common", Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UStatsComponent* StatComp;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	UTargetingComponent* TargetingComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* LockOnWidget;

	FName PelvisBoneName;

	FTimerHandle DestroyDeadTimerHandle; //죽은 캐릭터, 무기 destory

	float DestroyDeadTime;

	float WalkSpeed;

	float JogSpeed;

	float SprintSpeed;

	float SprintStaminaCost;

	bool bSprintKeyPressed;

	bool bHitFront; //맞았을 때 때린 캐릭터가 내 앞에서 때렸는지?
	
public: //get
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UCombatComponent* GetCombatComp() const { return CombatCompo; }
	FORCEINLINE UStateManagerComponent* GetStateManagerComp() const { return StateManagerComp; }
	FORCEINLINE float GetSprintStaminaCost() const { return SprintStaminaCost; }
	FORCEINLINE UStatsComponent* GetStatComp() const { return StatComp; }

public:
	void HeavyAttack();
	void LightAttack();
	void ChargedAttack();
	void SetMovementType(EMovementType Type);
	
};

