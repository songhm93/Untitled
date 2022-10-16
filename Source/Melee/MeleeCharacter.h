#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/CombatInterface.h"
#include "Interface/TargetingInterface.h"
//#include "Engine/DataTable.h"
#include "Type/Types.h"
#include "Type/DamageTypes.h"
#include "MeleeCharacter.generated.h"

DECLARE_DELEGATE_OneParam( FOnSprintState, bool );

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


// USTRUCT(BlueprintType)
// struct FCommonTable : public FTableRowBase
// {
// 	GENERATED_BODY()

// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	UAnimMontage* HitReactFrontMontage;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	UAnimMontage* HitReactBackMontage;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	UAnimMontage* KnockdownFrontMontage;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	UAnimMontage* KnockdownBackMontage;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	UAnimMontage* DodgeMontage;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	USoundCue* ImpactSound;
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	UParticleSystem* ImpactParticle;
	
// }; /** 데이터 테이블 추가시 에디터 끄고 컴파일 할 것 */


UCLASS(config=Game)
class AMeleeCharacter : public ACharacter, public ICombatInterface, public ITargetingInterface
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
	virtual float PerformCombatAction(ECharacterAction Action, ECharacterState State) override;
	UFUNCTION(BlueprintCallable)
	virtual void PerformLightAttack(int32 AttackCount) override;

	FOnSprintState OnSprintState;
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
	void PerformAttack(ECharacterAction Action);
	
	bool CanAttack();
	bool CanToggleCombat();
	void Dodge();
	void PerformDodge();
	bool CanDodge();
	void PerformAction(int32 MontageIdx, TArray<UAnimMontage*> Montage, ECharacterState State = ECharacterState::GENERAL_STATE, ECharacterAction Action = ECharacterAction::GENERAL_ACTION);
	void ToggleWalk();
	void SprintButtonPressed();
	void SprintButtonReleased();
	void HeavyAttack();
	
private:
	//void InitDataTable(FString Path, EDataTableType TableType);
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
	
	/**************   데이터 테이블 변수들 */

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
	void CharacterStateBegin(ECharacterState State);
	UFUNCTION()
	void CharacterStateEnd(ECharacterState State);
	UFUNCTION()
	void CharacterActionBegin(ECharacterAction Action);
	UFUNCTION()
	void CharacterActionEnd(ECharacterAction Action);

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

	//float AttackActionCorrectionValue; //공격 액션마다의 보정치. 1타, 2타, 3타있으면 3타가 가장 세게 적용.
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
	FORCEINLINE float GetAttackActionCorrectionValue() const { return AttackActionCorrectionValue; }
public: //set
	void SetMovementType(EMovementType Type);
public:
	void LightAttack();
	void ChargedAttack();
	void Equip(ABaseEquippable* Weapon);
	
	
};

