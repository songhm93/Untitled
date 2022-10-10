#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatInterface.h"
#include "Engine/DataTable.h"
#include "Types.h"
#include "MeleeCharacter.generated.h"


class AToughSword;
class ABaseWeapon;
class UCombatComponent;
class UStateManagerComponent;
class UDamageType;
class AController;
class UPrimitiveComponent;
class USoundCue;
class UParticleSystem;
class UStatsComponent;
class ABaseEquippable;

USTRUCT(BlueprintType)
struct FCommonTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* DodgeMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ChargedAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* ImpactSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* ImpactParticle;
	
}; /** 데이터 테이블 추가시 에디터 끄고 컴파일 할 것 */

USTRUCT(BlueprintType)
struct FLightSwordTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> LightAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* HeavyAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* SprintAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* EnterCombatMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ExitCombatMontage;
	
}; /** 데이터 테이블 추가시 에디터 끄고 컴파일 할 것 */

USTRUCT(BlueprintType)
struct FGreatSwordTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> LightAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* HeavyAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* EnterCombatMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ExitCombatMontage;
	
}; /** 데이터 테이블 추가시 에디터 끄고 컴파일 할 것 */

USTRUCT(BlueprintType)
struct FDualSwordTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> LightAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* HeavyAttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* EnterCombatMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* ExitCombatMontage;
	
}; /** 데이터 테이블 추가시 에디터 끄고 컴파일 할 것 */

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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
	void ToggleCombat();
	void InteractButtonPressed();
	void PerformAttack(int32 AttackIdx, bool bRandomIdx, ECharacterAction Action);
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
	void InitDataTable(FString Path, EDataTableType TableType);
	void Test(); //테스트할 함수
	
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
	UPROPERTY()
	UAnimMontage* HitReactMontage;
	UPROPERTY()
	UAnimMontage* DodgeMontage; 
	UPROPERTY()
	UAnimMontage* ChargedAttackMontage;
	UPROPERTY()
	USoundCue* ImpactSound;
	UPROPERTY()
	UParticleSystem* ImpactParticle;

	//Light Sword
	UPROPERTY()
	TArray<UAnimMontage*> LSLightAttackMontages;
	UPROPERTY()
	UAnimMontage* LSHeavyAttackMontage;
	UPROPERTY()
	UAnimMontage* LSSprintAttackMontage;
	UPROPERTY()
	UAnimMontage* LSEnterCombatMontage;
	UPROPERTY()
	UAnimMontage* LSExitCombatMontage;

	//Great Sword
	UPROPERTY()
	TArray<UAnimMontage*> GSLightAttackMontages; 
	UPROPERTY()
	UAnimMontage* GSHeavyAttackMontage;
	UPROPERTY()
	UAnimMontage* GSEnterCombatMontage;
	UPROPERTY()
	UAnimMontage* GSExitCombatMontage;

	//Dual Sword
	UPROPERTY()
	TArray<UAnimMontage*> DSLightAttackMontages;
	UPROPERTY()
	UAnimMontage* DSHeavyAttackMontage;
	UPROPERTY()
	UAnimMontage* DSEnterCombatMontage;
	UPROPERTY()
	UAnimMontage* DSExitCombatMontage;
	

	/*****************************************************/
;
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

	float AttackActionCorrectionValue; //공격 액션마다의 보정치. 1타, 2타, 3타있으면 3타가 가장 세게 적용.
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

