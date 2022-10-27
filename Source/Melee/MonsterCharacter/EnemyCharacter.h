#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interface/CombatInterface.h"
#include "../Interface/TargetingInterface.h"
#include "../Type/DamageTypes.h"
#include "../Type/Types.h"
#include "../Type/Elements.h"
#include "EnemyCharacter.generated.h"

UENUM(BlueprintType)
enum class EEnemyActionType : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    CLOSE_RANGE_ATTACK UMETA(DisplayName="CLOSE_RANGE_ATTACK"),
    DODGE UMETA(DisplayName="DODGE"),
	ENTER_COMBAT UMETA(DisplayName="ENTER_COMBAT"),
	EXIT_COMBAT UMETA(DisplayName="EXIT_COMBAT"),
    
    MAX UMETA(DisplayName="MAX")
};

class UParticleSystem;
class UAnimMontage;
class USoundCue;
class UBehaviorTree;
class UStateManagerComponent;
class UWidgetComponent;
class UMonsterStatsComponent;
class ATargetPoint;
class USphereComponent;
class AEnemyAIController;
class UEnemyAnimInstance;
class UMonstersCombatComponent;


UCLASS()
class MELEE_API AEnemyCharacter : public ACharacter, public ICombatInterface, public ITargetingInterface
{
	GENERATED_BODY()

public:
	AEnemyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual bool CanRecieveDamage() override;
	UFUNCTION(BlueprintCallable)
	virtual void ResetCombat() override;
	virtual bool CanBeTargeted() override;
	virtual void OnTargeted(bool IsTargeted) override;
	virtual void CalcReceiveDamage(float ATK) override;
	virtual void ApplyHitReaction(EDamageType DamageType) override;
	virtual void ApplyImpactEffect(EDamageType DamageType, FVector HitLocation) override;

protected:
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	UMonsterStatsComponent* MonsterStatComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true") )
	USphereComponent* AgroRangeSphere;
	UPROPERTY(EditAnywhere,  Meta = (AllowPrivateAccess = "true"))
	float AgroRange;
	UPROPERTY()
	UEnemyAnimInstance* EnemyAnimInst;
	UPROPERTY()
	AEnemyAIController* AIController;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UStateManagerComponent* StateManagerComp;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	float AgroCancelTime;
	FTimerHandle AgroCancelTimerHandle;
	bool bTargetingState;
	UPROPERTY()
	AActor* Target;
	UFUNCTION()
	virtual void ReceiveDamage(
		AActor* DamagedActor, 
		float EnemyATK, 
		AController* InstigatedBy, 
		FVector HitLocation, 
		UPrimitiveComponent* FHitComponent, 
		FName BoneName, 
		FVector ShotFromDirection, 
		const UDamageType* DamageType, 
		AActor* DamageCauser);
	virtual void Dead();
private:
	

	void EnableRagdoll();

	void ApplyHitReactionPhysicsVelocity(float InitSpeed);

	void DestroyDead();

	void LookAtPlayer(AActor* Player, float DeltaTime);

	UFUNCTION()
	void CharacterStateBegin(ECurrentState State);
	
	UPROPERTY(EditAnywhere, Category = "Common", Meta = (AllowPrivateAccess = "true"))
	USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Common", Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactFrontMontage;

	UPROPERTY(EditAnywhere, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactBackMontage;

	UPROPERTY(EditAnywhere, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KnockdownFrontMontage;

	UPROPERTY(EditAnywhere, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KnockdownBackMontage;

	UPROPERTY(EditAnywhere, Category = "BT", Meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* LockOnWidget;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol", Meta = (AllowPrivateAccess = "true"))
	TArray<ATargetPoint*> PatrolPoints;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 AttackMontageSectionNum;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UMonstersCombatComponent* MonsterCombatCompo;

	UPROPERTY(VisibleAnywhere, Category = "Element", Meta = (AllowPrivateAccess = "true"))
	EElements CurrentElement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	float AttackRange;

	EEnemyActionType EnemyActionType;

	float AttackActionCorrectionValue;
	
	bool bHitFront;
	
	FName PelvisBoneName;

	FTimerHandle DestroyDeadTimerHandle; //죽은 캐릭터, 무기 destory

	float DestroyDeadTime;


	
public: //get
	FORCEINLINE	UBehaviorTree* GetBT() const { return BehaviorTree; }
	FORCEINLINE UStateManagerComponent* GetStateManagerComp() const { return StateManagerComp; }
	FORCEINLINE TArray<ATargetPoint*> GetPatrolPoints() const { return PatrolPoints; }
	FORCEINLINE float GetAttackRange() const { return AttackRange; }
	FORCEINLINE UMonsterStatsComponent* GetMonsterStatComp() const { return MonsterStatComp; }

public:
	void AgroCancel();	
	
};
