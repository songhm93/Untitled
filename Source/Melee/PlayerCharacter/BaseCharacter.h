#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interface/CombatInterface.h"
#include "../Interface/TargetingInterface.h"
#include "../Interface/InventoryInterface.h"
#include "../Interface/QuestInterface.h"
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
class ADualWeapon;
class UInventoryComponent;
class UMainHUDWidget;
class UGetItemWidget;
class UQuestLogComponent;

UCLASS(config=Game)
class ABaseCharacter : public ACharacter, public ICombatInterface, public ITargetingInterface, public IInventoryInterface, public IQuestInterface
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
	
	virtual void CalcReceiveDamage(float ATK) override;
	virtual void ApplyHitReaction(EDamageType DamageType) override;
	virtual void ApplyImpactEffect() override;
	virtual bool AddItem(int32 ItemId, int32 Amount, bool bFromMonster) override;
	virtual void AddGold(int32 GoldAmount, bool bFromMonster) override;
	virtual int32 DeathMob(int32 MId) override;
	virtual void PlusCollectCurrentNum(int32 Amount) override;

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

	UFUNCTION()
	void EquipWeapon(int32 ItemId);

	UFUNCTION()
	void EquipArmor(int32 ItemId);

	UFUNCTION()
	void EquippedWeaponSpawn(int32 ItemId);
	
	UFUNCTION()
	void EquippedArmorApply(int32 ItemId);

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

	void HotkeySlot1ButtonPressed();

	void HotkeySlot2ButtonPressed();

	void HotkeySlot3ButtonPressed();

	void HotkeySlot4ButtonPressed();

	void LeftClickPressed();

	void LeftClickReleased();

	void ToggleInventory();

	void AltPressed();

	void AltReleased();

	UFUNCTION(BlueprintCallable)
	bool CanExecuteSkill();

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetPhysicsSurface();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", Meta = (AllowPrivateAccess = "true"))
	UStateManagerComponent* StateManagerComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", Meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* InventoryComp;

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
	UWidgetComponent* LockOnWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* InventoryWidgetComp;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMainHUDWidget> MainHUDClass;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UMainHUDWidget* MainHUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UQuestLogComponent* QuestLogComp;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	bool bAltPressed;

	FName PelvisBoneName;

	FTimerHandle DestroyDeadTimerHandle; //죽은 캐릭터, 무기 destory

	float DestroyDeadTime;

	float WalkSpeed;

	float JogSpeed;

	float SprintSpeed;

	float SprintStaminaCost;

	bool bSprintKeyPressed;

	bool bHitFront; //맞았을 때 때린 캐릭터가 내 앞에서 때렸는지?

	bool bLeftClicked;
	
public: //get
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UCombatComponent* GetCombatComp() const { return CombatCompo; }
	FORCEINLINE UStateManagerComponent* GetStateManagerComp() const { return StateManagerComp; }
	FORCEINLINE float GetSprintStaminaCost() const { return SprintStaminaCost; }
	FORCEINLINE UStatsComponent* GetStatComp() const { return StatComp; }
	FORCEINLINE UInventoryComponent* GetInventoryComp() const { return InventoryComp; }
	FORCEINLINE UMainHUDWidget* GetMainHUDWidget() const { return MainHUDWidget; }
	FORCEINLINE UQuestLogComponent* GetQuestLogComp() const { return QuestLogComp; }

public:
	void HeavyAttack();
	void LightAttack();
	void ChargedAttack();
	void SetMovementType(EMovementType Type);
	void AddExp(int32 Exp);
	
};

