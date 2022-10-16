#include "EnemyCharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Widget/EnemyHPBarWidget.h"
#include "Blueprint/UserWidget.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/TargetingInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "EnemyAnimInstance.h"
#include "EnemyAIController.h"
#include "AttackDamageType.h"
#include "Component/StateManagerComponent.h"
#include "Component/StatsComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
    AttackActionCorrectionValue = 1.f;
    StateManagerComp = CreateDefaultSubobject<UStateManagerComponent>(TEXT("StateManagerrComp"));
    LockOnWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOnWidget"));
	LockOnWidget->SetupAttachment(GetMesh());
    HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
    HPBarWidget->SetupAttachment(GetMesh());
    StatComp = CreateDefaultSubobject<UStatsComponent>(TEXT("StatComp"));
	PelvisBoneName = TEXT("pelvis");
	DestroyDeadTime = 4.f;
	HideHPBarTime = 3.f;
	AgroRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroRangeSphere"));
	AgroRange = 400.f;
	AgroRangeSphere->SetSphereRadius(AgroRange);
	AgroCancelTime = 3.5f;
	AgroRangeSphere->SetupAttachment(RootComponent);
	AttackActionCorrectionValue = 1.f;
	AttackMontageSectionNum = 3;
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	AIController = Cast<AEnemyAIController>(GetController());
	
	bTargetingState = false;
	ReadyToAttackTime = 3.f;
	bCanAttack = true;
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bTargetingState && Target && !bCanAttack)
	{
		LookAtPlayer(Target, DeltaTime);
	}
	

}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

	if(StateManagerComp)
	{
		StateManagerComp->OnStateBegin.AddDynamic(this, &ThisClass::CharacterStateBegin);
	}

    if(LockOnWidget)
	{
		LockOnWidget->SetVisibility(false);
		LockOnWidget->SetWidgetSpace(EWidgetSpace::Screen);
		LockOnWidget->SetDrawSize(FVector2D(14.f, 14.f));
	}

	OnTakePointDamage.AddDynamic(this, &ThisClass::ReceiveDamage);

	if(AgroRangeSphere)
	{
		AgroRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::AgroSphereBeginOverlap);
		AgroRangeSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::AgroSphereEndOverlap);
	}
	
	if(StatComp)
	{
		StatComp->InitStats();
		StatComp->PlusCurrentStatValue(EStats::HP, 0.00000001f);
	}

	if(HPBarWidget)
	{
		UUserWidget* EnemyHPBarWidget = CreateWidget<UEnemyHPBarWidget>(GetWorld(), HPBarWidget->GetWidgetClass());
		if(EnemyHPBarWidget)
		{
			HPBarWidget->SetWidget(EnemyHPBarWidget);
			HPBarWidget->SetVisibility(false);
			Cast<UEnemyHPBarWidget>(EnemyHPBarWidget)->Init(StatComp);
		}
	}

	if(Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Cast<UEnemyAnimInstance>(GetMesh()->GetAnimInstance())->OnApplyDamage.BindUObject(this, &ThisClass::DamageThePlayer);
	}
}

FName AEnemyCharacter::GetAttackSectionName()
{
    int32 RandomValue = FMath::RandRange(0, 2);
    FName ReturnValue = TEXT("");
    if(RandomValue == 0) 
	{
		AttackActionCorrectionValue = 1.f;
		ReturnValue = TEXT("First");
	}
	else if(RandomValue == 1) 
	{
		AttackActionCorrectionValue = 2.f;
		ReturnValue = TEXT("Second");
	}
	else if(RandomValue == 2) 
	{
		AttackActionCorrectionValue = 3.f;
		ReturnValue = TEXT("Third");
	}
    return ReturnValue;
}

bool AEnemyCharacter::CanRecieveDamage()
{
    if(StateManagerComp && StateManagerComp->GetCurrentState() != ECharacterState::DEAD)
		return true;
	else 
		return false;
}

void AEnemyCharacter::ContinueAttack()
{

}

void AEnemyCharacter::ResetAttack()
{

}

FRotator AEnemyCharacter::GetDesiredRotation()
{
    return FRotator::ZeroRotator;
}

void AEnemyCharacter::ResetCombat() 
{

}

bool AEnemyCharacter::CanBeTargeted()
{
    if(StateManagerComp)
	{
		TArray<ECharacterState> State;
		State.Add(ECharacterState::DEAD);
		return !StateManagerComp->IsCurrentStateEqualToThis(State);
	}
	return false;
}

void AEnemyCharacter::OnTargeted(bool IsTargeted)
{
    if(LockOnWidget && HPBarWidget)
	{
		LockOnWidget->SetVisibility(IsTargeted);

		if(IsTargeted)
			HPBarWidget->SetVisibility(IsTargeted);
		else
			GetWorldTimerManager().SetTimer(HideHPBarTimerHandle, this, &ThisClass::HideHPBar, HideHPBarTime);
		
	}
}

void AEnemyCharacter::HideHPBar()
{
	if(HPBarWidget)
	{
		HPBarWidget->SetVisibility(false);
	}
}

void AEnemyCharacter::ReceiveDamage(
	AActor* DamagedActor, 
	float EnemyATK,
	AController* InstigatedBy, 
	FVector HitLocation, 
	UPrimitiveComponent* FHitComponent, 
	FName BoneName, 
	FVector ShotFromDirection, 
	const UDamageType* DamageType, 
	AActor* DamageCauser)
{
	if(InstigatedBy)
	{
		float DotProductResult = GetDotProductTo(InstigatedBy->GetPawn()); //맞은 캐릭터와 때린 캐릭터간의 내적
		bHitFront = FMath::IsWithin(DotProductResult, -0.1f, 1.f);

		if(DamageType)
		{
			ApplyHitReaction(Cast<UAttackDamageType>(DamageType)->GetDamageType());
			ApplyImpactEffect(Cast<UAttackDamageType>(DamageType)->GetDamageType(), HitLocation);
		}

		if(StateManagerComp)
			StateManagerComp->SetCurrentState(ECharacterState::DISABLED);
		
		CalcReceiveDamage(EnemyATK);
	}
}

void AEnemyCharacter::CalcReceiveDamage(float EnemyATK) //받는 총 대미지 계산
{
	//대미지 계산
	if(StatComp)
	{
		const float Def = StatComp->GetCurrentStatValue(EStats::DEF);
		const float Result = FMath::Clamp((EnemyATK * FMath::RandRange(0.8, 1.2)) * (1 - (Def / (100 + Def))), 0, INT_MAX);
		StatComp->PlusCurrentStatValue(EStats::HP, Result * -1); //HP 적용
		if(StatComp->GetCurrentStatValue(EStats::HP) <= 0)
		{
			if(StateManagerComp)
				StateManagerComp->SetCurrentState(ECharacterState::DEAD);
		}
	}
}

void AEnemyCharacter::ApplyImpactEffect(EDamageType DamageType, FVector HitLocation)
{
	if(ImpactSound && ImpactParticle)
	{
		switch (DamageType)
		{
		case EDamageType::MELEE_DAMAGE:
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, HitLocation); 
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, HitLocation);
			break;
		case EDamageType::KNOCKDOWN_DAMAGE:
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, HitLocation); //나중에 다른걸로 추가
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, HitLocation);
			break;
		}
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

void AEnemyCharacter::CharacterStateBegin(ECharacterState State)
{
	switch (State)
	{
	case ECharacterState::NOTHING:
		
		break;
	case ECharacterState::ATTACKING:
		
		break;
	case ECharacterState::DODGING:
		
		break;
	case ECharacterState::GENERAL_STATE:
		
		break;
	case ECharacterState::DEAD:
			Dead();
		break;
	case ECharacterState::DISABLED:
		
		break;
	}
}

void AEnemyCharacter::Dead()
{
	EnableRagdoll();
	ApplyHitReactionPhysicsVelocity(2000.f);
	
	if(HPBarWidget)
		HPBarWidget->SetVisibility(false);
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

void AEnemyCharacter::AgroSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	AIController = AIController == nullptr ? Cast<AEnemyAIController>(GetController()) : AIController;

	if(OtherActor && OtherActor->Implements<UTargetingInterface>() && !(Cast<AEnemyCharacter>(OtherActor)) && AIController)
	{
		if(AIController)
		{
			AIController->GetBBComp()->SetValueAsObject(TEXT("Target"), OtherActor);
			bTargetingState = true;
			Target = OtherActor;
		}
		if(GetWorldTimerManager().IsTimerActive(AgroCancelTimerHandle))
		{
			GetWorldTimerManager().ClearTimer(AgroCancelTimerHandle);
		}
	}
}

void AEnemyCharacter::AgroSphereEndOverlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	AIController = AIController == nullptr ? Cast<AEnemyAIController>(GetController()) : AIController;
	
	if(OtherActor && OtherActor->Implements<UTargetingInterface>() && !(Cast<AEnemyCharacter>(OtherActor)) && AIController)
	{
		if(AIController)
		{
			GetWorldTimerManager().SetTimer(AgroCancelTimerHandle, this, &ThisClass::AgroCancel, AgroCancelTime);
		}
	}
}

void AEnemyCharacter::AgroCancel()
{
	AIController = AIController == nullptr ? Cast<AEnemyAIController>(GetController()) : AIController;
	
	if(AIController)
	{
		AIController->GetBBComp()->SetValueAsObject(TEXT("Target"), nullptr);
		bTargetingState = false;
		Target = nullptr;
	} 
}



float AEnemyCharacter::PerformCombatAction(ECharacterAction Action, ECharacterState State)
{
	return 0.f;
}

void AEnemyCharacter::PerformLightAttack(int32 AttackCount)
{
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	FName SectionName = GetLightAttackSectionName(AttackCount);

	if(StateManagerComp && AnimInst)
	{
		if(CloseRangeAttackMontage)
		{
			AnimInst->Montage_Play(CloseRangeAttackMontage);
			AnimInst->Montage_JumpToSection(SectionName, CloseRangeAttackMontage);
		}
		
		StateManagerComp->SetCurrentState(ECharacterState::ATTACKING);
		StateManagerComp->SetCurrentAction(ECharacterAction::LIGHT_ATTACK);

		bCanAttack = false;
		AIController = AIController == nullptr ? Cast<AEnemyAIController>(GetController()) : AIController;
		if(AIController && AIController->GetBBComp())
		{
			AIController->GetBBComp()->SetValueAsBool("CanAttack" , false);
		}
		GetWorldTimerManager().SetTimer(ReadyToAttackTimerHandle, this, &ThisClass::ReadyToAttack, ReadyToAttackTime);
	}
}

FName AEnemyCharacter::GetLightAttackSectionName(int32 AttackCount)
{
	if(AttackCount == 1) 
	{
		AttackActionCorrectionValue = 1.f;
		return TEXT("First");
	}
	else if(AttackCount == 2) 
	{
		AttackActionCorrectionValue = 2.f;
		return TEXT("Second");
	}
	else if(AttackCount == 3) 
	{
		AttackActionCorrectionValue = 3.f;
		return TEXT("Third");
	}
	return NAME_None;
}

void AEnemyCharacter::LookAtPlayer(AActor* Player, float DeltaTime)
{
	if(Player)
	{
		const FVector PlayerLocation = Player->GetActorLocation();
		const FVector EnemyLocation = GetActorLocation();
		const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(EnemyLocation, PlayerLocation);
		FRotator ResultRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 3.f);
		SetActorRotation(ResultRotation);
	}
}

void AEnemyCharacter::ReadyToAttack()
{
	bCanAttack = true;
	if(AIController && AIController->GetBBComp())
	{
		AIController->GetBBComp()->SetValueAsBool("CanAttack" , true);
	}
}

void AEnemyCharacter::DamageThePlayer()
{
	//여기서 라인트레이스로 앞을 긁어서 충돌한 플레이어 모두 대미지를 줄지, 그냥 타겟으로 지정한 애만 대미지를 줄지.
	
}