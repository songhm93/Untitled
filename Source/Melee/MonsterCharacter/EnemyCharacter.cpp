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
#include "Engine/TargetPoint.h"

#include "EnemyAnimInstance.h"
#include "../AttackDamageType.h"
#include "../Interface/TargetingInterface.h"
#include "../Interface/InventoryInterface.h"
#include "../Interface/QuestInterface.h"
#include "../Widget/EnemyHPBarWidget.h"
#include "../PlayerController/EnemyAIController.h"
#include "../Component/StateManagerComponent.h"
#include "../Component/MonsterStatsComponent.h"
#include "../Component/MonstersCombatComponent.h"
#include "../Type/Elements.h"
#include "../PlayerCharacter/BaseCharacter.h"
#include "../Equipment/BaseWeapon.h"



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
	AreaAgroCancelTime = 3.5f;
	AgroRangeSphere->SetupAttachment(RootComponent);
	AttackMontageSectionNum = 3;
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	AIController = Cast<AEnemyAIController>(GetController());
	MonsterCombatCompo = CreateDefaultSubobject<UMonstersCombatComponent>(TEXT("MonstersCC"));
	bTargetingState = false;
	CurrentElement = EElements::NONE;
	RespawnTime = 5.f;

	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
    HPBarWidget->SetupAttachment(GetMesh());
	HideHPBarTime = 3.f;

	IsBossFurious = false;
	DefaultATK = 0;
	DefaultDEF = 0;
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
		StateManagerComp->OnCombatState.AddUObject(this, &ThisClass::HPBarOnOff);
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
		MonsterStatComp->PlusCurrentStatValue(EStats::HP, 0.00000001f);
	}

	if(HPBarWidget)
	{
		EnemyHPBarWidget = CreateWidget<UEnemyHPBarWidget>(GetWorld(), HPBarWidget->GetWidgetClass());
		if(EnemyHPBarWidget)
		{
			HPBarWidget->SetWidget(EnemyHPBarWidget);
			HPBarWidget->SetVisibility(false);
			Cast<UEnemyHPBarWidget>(EnemyHPBarWidget)->Init(MonsterStatComp);
			Cast<UEnemyHPBarWidget>(EnemyHPBarWidget)->SetMonsterName(MonsterName);
		}
	}
}

bool AEnemyCharacter::CanReceiveDamage()
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
	if(!Cast<ABaseWeapon>(DamageCauser) && !Cast<ABaseCharacter>(DamageCauser)) return;
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

bool AEnemyCharacter::CalcCritical(float Percent)
{
	bool ReturnValue = false;
	const float RandValue = FMath::RandRange(0.f, 999.f);
	const float MaxRangeValue = Percent * 10.f;

	if(MaxRangeValue >= RandValue)
	{
		ReturnValue = true;
	}

	return ReturnValue;
}

void AEnemyCharacter::CalcReceiveDamage(float PlayerATK) //받는 총 대미지 계산
{
	bool IsCritical = CalcCritical(10.f);
	
	if(MonsterStatComp)
	{
		const float Def = MonsterStatComp->GetCurrentStatValue(EStats::DEF);
		float Result = FMath::Clamp((PlayerATK * FMath::RandRange(0.8, 1.2)) * (1 - (Def / (100 + Def))), 0, INT_MAX);
		if(IsCritical) Result *= 2.f;
		MonsterStatComp->PlusCurrentStatValue(EStats::HP, Result * -1); //HP 적용

		int32 CurrentHP = MonsterStatComp->GetCurrentStatValue(EStats::HP);

		if(MonsterType == EMonsterType::BOSS)
		{
			int32 MaxHP = MonsterStatComp->GetMaxValue(EStats::HP);
			int32 FuriousHP = MaxHP - (MaxHP * 0.6);
			if((CurrentHP < FuriousHP) && !IsBossFurious) //분노
			{
				IsBossFurious = true;
				BossFurious();
			}
		}
		
		if(CurrentHP <= 0)
		{
			if(StateManagerComp)
				StateManagerComp->SetCurrentState(ECurrentState::DEAD);
			if(MonsterType == EMonsterType::BOSS)
			{
				FuriousDead();
			}
		}
		//Result로 대미지 위젯
		ShowDamageText(Result, IsCritical);
	}
}

void AEnemyCharacter::BossFurious()
{
	if(MonsterStatComp)
	{
		DefaultATK = MonsterStatComp->GetCurrentStatValue(EStats::ATK);
		DefaultDEF = MonsterStatComp->GetCurrentStatValue(EStats::DEF);

		MonsterStatComp->SetCurrentStatValue(EStats::ATK, DefaultATK + DefaultATK * 0.2);
		MonsterStatComp->SetCurrentStatValue(EStats::DEF, DefaultDEF + DefaultDEF);
	}
	//액터 스폰
	//BP에서 BeginPlay에서 PP 설정
	SetFuriousPP();
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
	CalcChance(5.f);
	
	if(Target && Target->Implements<UInventoryInterface>() && MonsterStatComp)
	{
		int32 IsCollectQuest = 0;
		if(Target->Implements<UQuestInterface>())
		{
			IsCollectQuest = Cast<IQuestInterface>(Target)->DeathMob(MId);
			if(IsCollectQuest != 0) //수집 퀘스트이면
			{
				int32 Amount = FMath::RandRange(0, 1);
				if(CalcChance(60.f) && Amount == 1)
				{
					Cast<IInventoryInterface>(Target)->AddItem(IsCollectQuest, Amount, true);
					Cast<IQuestInterface>(Target)->PlusCollectCurrentNum(Amount);
				}
			}
			
			for(auto GivesItem : MonsterStatComp->GetMonsterGives().ItemId)
			{
				int32 Amount = FMath::RandRange(0, 1);
				if(CalcChance(40.f)  && Amount == 1)
				{
					Cast<IInventoryInterface>(Target)->AddItem(GivesItem, Amount, true);
				}
			}	
		}
		if(CalcChance(40.f))
		{
			int32 RandValue = FMath::RandRange(0, 10);
			if(CalcChance(50.f))
			{
				Cast<IInventoryInterface>(Target)->AddGold(MonsterStatComp->GetMonsterGives().Gold + RandValue, true);
			}
			else
			{
				Cast<IInventoryInterface>(Target)->AddGold(MonsterStatComp->GetMonsterGives().Gold - RandValue, true);
			}
			
		}
		
		if(Cast<ABaseCharacter>(Target))
			Cast<ABaseCharacter>(Target)->AddExp(MonsterStatComp->GetMonsterGives().Exp);
	}
	AgroCancel();
	
	if(GetCharacterMovement())
	{
		GetCharacterMovement()->Deactivate();
	}
	if(DeathMontage && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage);
	}
	if(AgroRangeSphere)
		AgroRangeSphere->Deactivate();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
	GetWorldTimerManager().SetTimer(DestroyDeadTimerHandle, this, &ThisClass::DestroyDead, DestroyDeadTime);
}

void AEnemyCharacter::DestroyDead()
{
	GetMesh()->SetVisibility(false);
	
	if(MonsterType == EMonsterType::BOSS) return;
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ThisClass::Respawn, RespawnTime);
}

void AEnemyCharacter::Respawn()
{
	
	if(MonsterStatComp)
	{
		MonsterStatComp->InitInfo();
	}
	if(StateManagerComp)
	{
		StateManagerComp->SetCurrentState(ECurrentState::GENERAL_STATE);
		StateManagerComp->SetCurrentAction(ECurrentAction::NOTHING);
		StateManagerComp->SetCurrentCombatState(ECurrentCombatState::NONE_COMBAT_STATE);
		AgroCancel();
	}
	if(!PatrolPoints.IsEmpty())
	{
		int32 RandNum = FMath::RandRange(0, PatrolPoints.Num() - 1);
		SetActorLocation(PatrolPoints[RandNum]->GetActorLocation() + FVector(0.f, 0.f, 50.f));
	}
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly); 
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly); 
	if(GetCharacterMovement())
	{
		GetCharacterMovement()->Activate();
	}
	if(AgroRangeSphere)
		AgroRangeSphere->Activate();
	GetMesh()->SetVisibility(true);
	if(RespawnParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RespawnParticle, GetActorLocation());
	}	
}

bool AEnemyCharacter::CalcChance(float Percent)
{
	bool ReturnValue = false;
	float RandValue = FMath::RandRange(0.f, 999.f);
	float MaxRangeValue = Percent * 10.f;

	if(MaxRangeValue >= RandValue)
	{
		ReturnValue = true;
	}

	return ReturnValue;
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
		AIController->GetBBComp()->SetValueAsBool(TEXT("InAttackRange"), false);
		AIController->GetBBComp()->SetValueAsBool(TEXT("SpecialComplete"), true);
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

	if(GetWorldTimerManager().IsTimerActive(AgroCancelTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(AgroCancelTimerHandle);
	}
	if(GetWorldTimerManager().IsTimerActive(AreaAgroCancelTimeHandle))
	{
		GetWorldTimerManager().ClearTimer(AreaAgroCancelTimeHandle);
	}
	
	if(AIController && StateManagerComp)
	{
		if(StateManagerComp->GetCurrentState() == ECurrentState::DEAD) return;

		AIController->GetBBComp()->SetValueAsObject(TEXT("Target"), Player);
		bTargetingState = true;
		Target = Player;
		StateManagerComp->SetCurrentCombatState(ECurrentCombatState::COMBAT_STATE);
		AIController->GetBBComp()->SetValueAsBool(TEXT("CombatState"), true);
	}
	
	
	if(MonsterType == EMonsterType::BOSS) return;
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

void AEnemyCharacter::AreaAgroCancel()
{
	ExitCombat(true);
}

void AEnemyCharacter::ExitCombat(bool First)
{
	AIController = AIController == nullptr ? Cast<AEnemyAIController>(GetController()) : AIController;
	
	
	if(AIController && StateManagerComp)
	{
		AIController->GetBBComp()->SetValueAsObject(TEXT("Target"), nullptr);
		bTargetingState = false;
		Target = nullptr;
		StateManagerComp->SetCurrentCombatState(ECurrentCombatState::NONE_COMBAT_STATE);
		AIController->GetBBComp()->SetValueAsBool(TEXT("CombatState"), false);
		AIController->GetBBComp()->SetValueAsBool(TEXT("InAttackRange"), false);
		OnTargeted(false);
	} 
	
	if(GetWorldTimerManager().IsTimerActive(AgroCancelTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(AgroCancelTimerHandle);
	}

	if(MonsterType == EMonsterType::BOSS) return;
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
					Cast<AEnemyCharacter>(Mob)->ExitCombat(false);
				}
			}
		}
	}
}

void AEnemyCharacter::BossAgroCancel()
{
	if(GetWorldTimerManager().IsTimerActive(AgroCancelTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(AgroCancelTimerHandle);
	}
	GetWorldTimerManager().SetTimer(AgroCancelTimerHandle, this, &ThisClass::AgroCancel, AgroCancelTime);
}

void AEnemyCharacter::HPBarOnOff(bool Show)
{
	if(HPBarWidget)
	{
		if(Show)
		{
			if(GetWorldTimerManager().IsTimerActive(HideHPBarTimerHandle))
				GetWorldTimerManager().ClearTimer(HideHPBarTimerHandle);
			
			if(StateManagerComp && StateManagerComp->GetCurrentState() == ECurrentState::DEAD)
			{
				HPBarWidget->SetVisibility(false);
				return;
			}
			
			HPBarWidget->SetVisibility(Show);
		}
		else
		{
			GetWorldTimerManager().SetTimer(HideHPBarTimerHandle, this, &ThisClass::HideHPBar, HideHPBarTime);
		}
	}
}

void AEnemyCharacter::HideHPBar()
{
	if(HPBarWidget)
	{
		HPBarWidget->SetVisibility(false);
	}
}

void AEnemyCharacter::SetMonsterName(FString Name)
{
	MonsterName = Name;
	if(Cast<UEnemyHPBarWidget>(EnemyHPBarWidget))
	{
		Cast<UEnemyHPBarWidget>(EnemyHPBarWidget)->SetMonsterName(MonsterName);
	}
}

void AEnemyCharacter::PlayCameraShake(bool IsDefault)
{
	if(DefaultCameraShakeClass && ExplodeCameraShakeClass)
	{
		if(IsDefault)
		{
			GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(DefaultCameraShakeClass);
		}
		else
		{
			GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(ExplodeCameraShakeClass);
		}
	}
}