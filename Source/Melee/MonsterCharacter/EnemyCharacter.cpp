#include "EnemyCharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "EnemyAnimInstance.h"
#include "../AttackDamageType.h"
#include "../Interface/TargetingInterface.h"
#include "../Interface/InventoryInterface.h"
#include "../Widget/EnemyHPBarWidget.h"
#include "../PlayerController/EnemyAIController.h"
#include "../Component/StateManagerComponent.h"
#include "../Component/MonsterStatsComponent.h"
#include "../Component/MonstersCombatComponent.h"
#include "../Type/Elements.h"



AEnemyCharacter::AEnemyCharacter()
{
    StateManagerComp = CreateDefaultSubobject<UStateManagerComponent>(TEXT("StateManagerrComp"));
    LockOnWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidget"));
	LockOnWidget->SetupAttachment(GetMesh());
    MonsterStatComp = CreateDefaultSubobject<UMonsterStatsComponent>(TEXT("MonsterStatComp"));
	PelvisBoneName = TEXT("pelvis");
	DestroyDeadTime = 4.f;
	AgroRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroRangeSphere"));
	AgroRange = 400.f;
	AgroRangeSphere->SetSphereRadius(AgroRange);
	AgroCancelTime = 3.5f;
	AgroRangeSphere->SetupAttachment(RootComponent);
	AttackMontageSectionNum = 3;
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	AIController = Cast<AEnemyAIController>(GetController());
	MonsterCombatCompo = CreateDefaultSubobject<UMonstersCombatComponent>(TEXT("MonstersCC"));
	bTargetingState = false;
	CurrentElement = EElements::NONE;
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(MonsterCombatCompo && bTargetingState && Target && !MonsterCombatCompo->CanAttack())
	{
		LookAtPlayer(Target, DeltaTime);
	}
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();


	if(StateManagerComp)
	{
		StateManagerComp->OnStateBegin.AddUObject(this, &ThisClass::CharacterStateBegin);
		StateManagerComp->SetCurrentState(ECurrentState::GENERAL_STATE);
		
	}

    if(LockOnWidget)
	{
		LockOnWidget->SetVisibility(false);
		LockOnWidget->SetWidgetSpace(EWidgetSpace::Screen);
		LockOnWidget->SetDrawSize(FVector2D(14.f, 14.f));
	}

	OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	
	if(MonsterStatComp)
	{
		MonsterStatComp->InitStats();
		MonsterStatComp->PlusCurrentStatValue(EStats::HP, 0.00000001f);
	}
}

bool AEnemyCharacter::CanRecieveDamage()
{
    if(StateManagerComp && StateManagerComp->GetCurrentState() != ECurrentState::DEAD)
		return true;
	else 
		return false;
}

void AEnemyCharacter::ResetCombat() 
{
	if(StateManagerComp)
	{
		StateManagerComp->SetCurrentState(ECurrentState::GENERAL_STATE);
		StateManagerComp->ResetAction();
	}
}

bool AEnemyCharacter::CanBeTargeted()
{
    if(StateManagerComp)
	{
		TArray<ECurrentState> State;
		State.Add(ECurrentState::DEAD);
		return !StateManagerComp->IsCurrentStateEqualToThis(State);
	}
	return false;
}

void AEnemyCharacter::OnTargeted(bool IsTargeted)
{
    if(LockOnWidget)
	{
		LockOnWidget->SetVisibility(IsTargeted);
	}
}


void AEnemyCharacter::ReceiveDamage(
	AActor* DamagedActor, 
	float EnemyATK, 
	const UDamageType* DamageType, 
	AController* InstigatedBy, 
	AActor* DamageCauser)
{
	if(InstigatedBy)
	{
		float DotProductResult = GetDotProductTo(InstigatedBy->GetPawn()); //맞은 캐릭터와 때린 캐릭터간의 내적
		bHitFront = FMath::IsWithin(DotProductResult, -0.1f, 1.f);

		if(Cast<UAttackDamageType>(DamageType))
		{
			ApplyHitReaction(Cast<UAttackDamageType>(DamageType)->GetDamageType());
			ApplyImpactEffect();
		}

		if(StateManagerComp)
			StateManagerComp->SetCurrentState(ECurrentState::DISABLED);
		
		CalcReceiveDamage(EnemyATK);
	}
}

void AEnemyCharacter::CalcReceiveDamage(float EnemyATK) //받는 총 대미지 계산
{
	//대미지 계산
	if(MonsterStatComp)
	{
		const float Def = MonsterStatComp->GetCurrentStatValue(EStats::DEF);
		const float Result = FMath::Clamp((EnemyATK * FMath::RandRange(0.8, 1.2)) * (1 - (Def / (100 + Def))), 0, INT_MAX);
		MonsterStatComp->PlusCurrentStatValue(EStats::HP, Result * -1); //HP 적용
		if(MonsterStatComp->GetCurrentStatValue(EStats::HP) <= 0)
		{
			if(StateManagerComp)
				StateManagerComp->SetCurrentState(ECurrentState::DEAD);
		}
	}
}

void AEnemyCharacter::ApplyImpactEffect()
{
	if(ImpactSound && ImpactParticle)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation()); 
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorLocation());
	}	
}

void AEnemyCharacter::ApplyHitReaction(EDamageType DamageType)
{
	if(bHitFront)
	{
		if(HitReactFrontMontage)
			PlayAnimMontage(HitReactFrontMontage);
	}
	else
	{
		if(HitReactBackMontage)
			PlayAnimMontage(HitReactBackMontage);
	}
}

void AEnemyCharacter::CharacterStateBegin(ECurrentState State)
{
	switch (State)
	{
	case ECurrentState::NOTHING:
		
		break;
	case ECurrentState::ATTACKING:
		
		break;
	case ECurrentState::DODGING:
		
		break;
	case ECurrentState::GENERAL_STATE:
		
		break;
	case ECurrentState::DEAD:
			Dead();
		break;
	case ECurrentState::DISABLED:
		
		break;
	}
}

void AEnemyCharacter::Dead()
{
	if(Target && Target->Implements<UInventoryInterface>())
	{
		//나를 죽인 플레이어에게 아이템 지급.
		//인터페이스로 해보자.
		//Cast<IInventoryInterface>(Target)->AddItem();
		//여기서 Delegate로 플레이어에게 내가 이 몬스터를 죽여서 아이템을 얻었다 라고 알림.
		//인벤을 열때마다 DB에서 가져와서 동기화하겠지만, 인벤을 연 상태에서
		//몬스터를 죽였을 때 비동기처럼 업데이트 되는것을 보이기 위해.
	}
	AgroCancel();
	EnableRagdoll();
	ApplyHitReactionPhysicsVelocity(2000.f);
	GetWorldTimerManager().SetTimer(DestroyDeadTimerHandle, this, &ThisClass::DestroyDead, DestroyDeadTime);
}

void AEnemyCharacter::EnableRagdoll()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	FAttachmentTransformRules Rules = FAttachmentTransformRules::KeepWorldTransform;
	GetMesh()->SetCollisionProfileName(TEXT("ragdoll"));
	GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBoneName, true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PelvisBoneName, 1.f);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	
}

void AEnemyCharacter::ApplyHitReactionPhysicsVelocity(float InitSpeed)
{
	const FVector NewVel = GetActorForwardVector() * (InitSpeed * -1.f);
	
	GetMesh()->SetPhysicsLinearVelocity(NewVel, false, PelvisBoneName);
}

void AEnemyCharacter::DestroyDead()
{
	Destroy();
}

void AEnemyCharacter::AgroCancel()
{
	AIController = AIController == nullptr ? Cast<AEnemyAIController>(GetController()) : AIController;
	
	if(AIController && StateManagerComp)
	{
		AIController->GetBBComp()->SetValueAsObject(TEXT("Target"), nullptr);
		bTargetingState = false;
		Target = nullptr;
		StateManagerComp->SetCurrentCombatState(ECurrentCombatState::NONE_COMBAT_STATE);
		AIController->GetBBComp()->SetValueAsBool(TEXT("CombatState"), false);
		OnTargeted(false);
	} 
}

void AEnemyCharacter::LookAtPlayer(AActor* Player, float DeltaTime)
{
	
	if(Player && Player->Implements<UTargetingInterface>() && !(Cast<ITargetingInterface>(Player)->CanBeTargeted()))
	{
		AgroCancel();
		return;
	}
	if(Player)
	{
		const FVector PlayerLocation = Player->GetActorLocation();
		const FVector EnemyLocation = GetActorLocation();
		const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(EnemyLocation, PlayerLocation);

		FRotator ResultRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 3.f);
		SetActorRotation(FRotator(0.f, ResultRotation.Yaw, 0.f));
	}
}

void AEnemyCharacter::EnterCombat(AActor* Player, bool First)
{
	AIController = AIController == nullptr ? Cast<AEnemyAIController>(GetController()) : AIController;
	
	if(AIController && StateManagerComp)
	{
		AIController->GetBBComp()->SetValueAsObject(TEXT("Target"), Player);
		bTargetingState = true;
		Target = Player;
		StateManagerComp->SetCurrentCombatState(ECurrentCombatState::COMBAT_STATE);
		AIController->GetBBComp()->SetValueAsBool(TEXT("CombatState"), true);
	}
	if(GetWorldTimerManager().IsTimerActive(AgroCancelTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(AgroCancelTimerHandle);
	}
	if(First)
	{
		TArray<AActor*> OutActor;
		UGameplayStatics::GetAllActorsOfClass(this, AEnemyCharacter::StaticClass(), OutActor);

		if(!OutActor.IsEmpty())
		{
			for(auto Mob : OutActor)
			{
				if(Cast<AEnemyCharacter>(Mob)->GetAreaNum() == AreaNum)
				{
					Cast<AEnemyCharacter>(Mob)->EnterCombat(Player, false);
				}
			}
		}
	}
	
}
