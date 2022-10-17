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
class UStatsComponent;
class ATargetPoint;
class USphereComponent;
class AEnemyAIController;
class UEnemyAnimInstance;
class UMonsterCombatComponent;


UCLASS()
class MELEE_API AEnemyCharacter : public ACharacter, public ICombatInterface, public ITargetingInterface
{
	GENERATED_BODY()

public:
	AEnemyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual bool CanRecieveDamage() override;
	virtual void ContinueAttack() override;
	virtual void ResetAttack() override;
	virtual FRotator GetDesiredRotation() override;
	virtual void ResetCombat() override;
	virtual bool CanBeTargeted() override;
	virtual void OnTargeted(bool IsTargeted) override;
	virtual float PerformCombatAction(ECurrentAction Action, ECurrentState State) override;
	UFUNCTION(BlueprintCallable)
	virtual void PerformLightAttack(int32 AttackCount) override;
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere, Category = "Common", Meta = (AllowPrivateAccess = "true"))
	USoundCue* ImpactSound;
	UPROPERTY(EditAnywhere, Category = "Common", Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle;

	
	UPROPERTY(EditAnywhere, Category= "AttackMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* CloseRangeAttackMontage;
	UPROPERTY(EditAnywhere, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactFrontMontage;
	UPROPERTY(EditAnywhere, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactBackMontage;
	UPROPERTY(EditAnywhere, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KnockdownFrontMontage;
	UPROPERTY(EditAnywhere, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* KnockdownBackMontage;

	EEnemyActionType EnemyActionType;
	FName GetAttackSectionName();
	float AttackActionCorrectionValue;
	UPROPERTY(EditAnywhere, Category = "BT", Meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTree;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UStateManagerComponent* StateManagerComp;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* LockOnWidget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* HPBarWidget;
	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	UStatsComponent* StatComp;

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
		
	void CalcReceiveDamage(float EnemyATK);
	bool bHitFront;
	void ApplyImpactEffect(EDamageType DamageType, FVector HitLocation);
	void ApplyHitReaction(EDamageType DamageType);
	UFUNCTION()
	void CharacterStateBegin(ECurrentState State);
	void Dead();
	void EnableRagdoll();
	void ApplyHitReactionPhysicsVelocity(float InitSpeed);
	FName PelvisBoneName;
	FTimerHandle DestroyDeadTimerHandle; //죽은 캐릭터, 무기 destory
	float DestroyDeadTime;
	void DestroyDead();
	void HideHPBar();
	FTimerHandle HideHPBarTimerHandle;
	float HideHPBarTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol", Meta = (AllowPrivateAccess = "true"))
	TArray<ATargetPoint*> PatrolPoints;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true") )
	USphereComponent* AgroRangeSphere;
	
	UPROPERTY(EditAnywhere,  Meta = (AllowPrivateAccess = "true"))
	float AgroRange;
	
	UFUNCTION()
	void AgroSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void AgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UPROPERTY()
	AEnemyAIController* AIController;

	FTimerHandle AgroCancelTimerHandle;
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	float AgroCancelTime;
	void AgroCancel();

	
	FName GetLightAttackSectionName(int32 AttackCount);
	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 AttackMontageSectionNum;
	
	bool bTargetingState;
	void LookAtPlayer(AActor* Player, float DeltaTime);
	UPROPERTY()
	AActor* Target;
	FTimerHandle ReadyToAttackTimerHandle;
	float ReadyToAttackTime;
	void ReadyToAttack();
	bool bCanAttack;
	UEnemyAnimInstance* EnemyAnimInst;
	void DamageThePlayer(); 
	
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UMonsterCombatComponent* MonsterCombatComp;

	UPROPERTY(VisibleAnywhere, Category = "Element", Meta = (AllowPrivateAccess = "true"))
	EElements CurrentElement;
public: //get
	FORCEINLINE	UBehaviorTree* GetBT() const { return BehaviorTree; }

	
};
