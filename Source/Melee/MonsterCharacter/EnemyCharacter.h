#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interface/CombatInterface.h"
#include "../Interface/TargetingInterface.h"
#include "../Type/DamageTypes.h"
#include "../Type/Types.h"
#include "../Type/Elements.h"
#include "EnemyCharacter.generated.h"


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
class UUserWidget;
class UCameraShakeBase;

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

UENUM(BlueprintType)
enum class EMonsterType : uint8
{
    NONE UMETA(DisplayName = "NONE"),
    NORMAL_MOB UMETA(DisplayName = "NORMAL_MOB"),
    BOSS UMETA(DisplayName = "BOSS"),

    MAX UMETA(DisplayName="MAX")
};



UCLASS()
class MELEE_API AEnemyCharacter : public ACharacter, public ICombatInterface, public ITargetingInterface
{
	GENERATED_BODY()

public:
	AEnemyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual bool CanReceiveDamage() override;
	UFUNCTION(BlueprintCallable)
	virtual void ResetCombat() override;
	virtual bool CanBeTargeted() override;
	virtual void OnTargeted(bool IsTargeted) override;
	virtual void CalcReceiveDamage(float ATK) override;
	virtual void ApplyHitReaction(EDamageType DamageType) override;
	virtual void ApplyImpactEffect() override;
	virtual bool CalcCritical(float Percent) override;
	virtual void Respawn();
	UFUNCTION(BlueprintImplementableEvent)
	void ShowDamageText(int32 Damage, bool IsCritical);
	void EnterCombat(AActor* Player, bool First);
	void ExitCombat(bool First);
	virtual void ActiveSquareArea(bool IsActive);
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Meta = (AllowPrivateAccess = "true"))
	UMonsterStatsComponent* MonsterStatComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = "true") )
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

	float AreaAgroCancelTime;

	FTimerHandle AreaAgroCancelTimeHandle;

	bool bTargetingState;

	UPROPERTY()
	AActor* Target;

	UFUNCTION()
	virtual void ReceiveDamage(
		AActor* DamagedActor, 
		float EnemyATK, 
		const UDamageType* DamageType, 
		AController* InstigatedBy, 
		AActor* DamageCauser);

	virtual void Dead();

	bool CalcChance(float Percent);

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	FString AreaNum;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	int32 MId;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* HPBarWidget;

	bool IsBossFurious;

	void BossFurious();

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstMaster;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstTorso;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstLegs;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstArms;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstMasterAnim;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterialInstArmsAnim;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* MaterialInstMaster;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* MaterialInstTorso;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* MaterialInstLegs;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* MaterialInstArms;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* MaterialInstMasterAnim;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* MaterialInstArmsAnim;

	float InterpResult;
private:
	void DestroyDead();

	void LookAtPlayer(AActor* Player, float DeltaTime);

	void DropItem();
	
	UPROPERTY(EditAnywhere, Category = "Common", Meta = (AllowPrivateAccess = "true"))
	USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Common", Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere, Category = "Common", Meta = (AllowPrivateAccess = "true"))
	UParticleSystem* RespawnParticle;

	UPROPERTY(EditAnywhere, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactFrontMontage;

	UPROPERTY(EditAnywhere, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactBackMontage;

	UPROPERTY(EditAnywhere, Category = "CommonMontage", Meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathMontage;

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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = "true"))
	EMonsterType MonsterType;

	FTimerHandle RespawnTimerHandle;

	float RespawnTime;

	FString MonsterName;

	FTimerHandle HideHPBarTimerHandle;

	float HideHPBarTime;
	
	void HPBarOnOff(bool Show);

	void HideHPBar();

	UPROPERTY()
	UUserWidget* EnemyHPBarWidget;

	float DefaultATK;

	float DefaultDEF;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> DefaultCameraShakeClass;

	UPROPERTY(EditAnywhere, Meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> ExplodeCameraShakeClass;
public: //get
	FORCEINLINE	UBehaviorTree* GetBT() const { return BehaviorTree; }
	FORCEINLINE UStateManagerComponent* GetStateManagerComp() const { return StateManagerComp; }
	FORCEINLINE TArray<ATargetPoint*> GetPatrolPoints() const { return PatrolPoints; }
	FORCEINLINE float GetAttackRange() const { return AttackRange; }
	FORCEINLINE UMonsterStatsComponent* GetMonsterStatComp() const { return MonsterStatComp; }
	FORCEINLINE FString GetAreaNum() const { return AreaNum; }
	FORCEINLINE int32 GetMId() const { return MId; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FString GetMonsterName() const { return MonsterName; }
	

public:
	void AgroCancel();	
	void AreaAgroCancel();
	void BossAgroCancel();
	void SetMonsterName(FString Name);
	UFUNCTION(BlueprintImplementableEvent)
	void SetFuriousPP();
	UFUNCTION(BlueprintImplementableEvent)
	void FuriousDead();
	void PlayCameraShake(bool IsDefault);
};
