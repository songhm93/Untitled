#include "MonstersCombatComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"

#include "../PlayerController/EnemyAIController.h"
#include "../MonsterCharacter/EnemyAnimInstance.h"
#include "../MonsterCharacter/EnemyCharacter.h"

UMonstersCombatComponent::UMonstersCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bEnemyWeaponCollisionEnabled = false;

	CloseAttackCorrectionValue = 1.f;
    ReadyToAttackTime = 3.f;
}


void UMonstersCombatComponent::BeginPlay()
{
	Super::BeginPlay();

    OwnerCharacter = Cast<ACharacter>(GetOwner());
    if(OwnerCharacter && OwnerCharacter->GetController() && OwnerCharacter->GetMesh())
	{
		AIController = Cast<AEnemyAIController>(OwnerCharacter->GetController());
		AnimInst = OwnerCharacter->GetMesh()->GetAnimInstance();
		if(Cast<UEnemyAnimInstance>(AnimInst))
		{
			Cast<UEnemyAnimInstance>(AnimInst)->OnImpact.BindUObject(this, &ThisClass::ImpactTrace);
		}
	} 
}


void UMonstersCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UMonstersCombatComponent::LightAttack()
{
	FName SectionName = GetLightAttackSectionName(1);

	if(AnimInst)
	{
		if(CloseRangeAttackMontage)
		{
			AnimInst->Montage_Play(CloseRangeAttackMontage);
			AnimInst->Montage_JumpToSection(SectionName, CloseRangeAttackMontage);
		}
		
		OnUpdateCurrentState.ExecuteIfBound(ECurrentState::ATTACKING);
		OnUpdateCurrentAction.ExecuteIfBound(ECurrentAction::LIGHT_ATTACK);

		bCanAttack = false;
		
		if(AIController && AIController->GetBBComp())
		{
			AIController->GetBBComp()->SetValueAsBool("CanAttack" , false);
		}
        if(GetWorld())
            GetWorld()->GetTimerManager().SetTimer(ReadyToAttackTimerHandle, this, &ThisClass::ReadyToAttack, ReadyToAttackTime);
	}
}

FName UMonstersCombatComponent::GetLightAttackSectionName(int32 Count)
{
	FName ReturnValue = TEXT("");
	int32 RandomValue = FMath::RandRange(0, 2);
	
    if (RandomValue == 0)
    {
        CloseAttackCorrectionValue = 1.f;
        ReturnValue = TEXT("First");
    }
    else if (RandomValue == 1)
    {
        CloseAttackCorrectionValue = 1.5f;
        ReturnValue = TEXT("Second");
    }
    else if (RandomValue == 2)
    {
        CloseAttackCorrectionValue = 2.f;
        ReturnValue = TEXT("Third");
    }
	
    return ReturnValue;
}

void UMonstersCombatComponent::ReadyToAttack()
{
	bCanAttack = true;
	if(AIController && AIController->GetBBComp())
	{
		AIController->GetBBComp()->SetValueAsBool("CanAttack" , true);
	}
}

void UMonstersCombatComponent::ImpactTrace()
{
	float AttackRange = Cast<AEnemyCharacter>(GetOwner())->GetAttackRange();
	const FVector Start = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * ( AttackRange * 0.5f );
	const FVector End = Start + GetOwner()->GetActorForwardVector() * 100.f;
	const FVector HalfSize = FVector(( AttackRange * 0.5f ), ( AttackRange * 0.5f ), 100.f);
	const FRotator Oritentation = GetOwner()->GetActorRotation();


	TArray<TEnumAsByte<EObjectTypeQuery>> CollisionObjectType;
	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	CollisionObjectType.Add(Pawn);

	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(GetOwner());
	TArray<FHitResult> OutHitResult;

	UKismetSystemLibrary::BoxTraceMultiForObjects(
		this,
		Start,
		End,
		HalfSize,
		Oritentation,
		CollisionObjectType,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Blue,
		5.f
	);

	ClearHitActors();

	if(!OutHitResult.IsEmpty())
	{
		for (auto LastHit : OutHitResult)
		{
			if (!AlreadyHitActors.Contains(LastHit.GetActor())) //중복이 아니면
			{
				HitFromDirection = GetOwner()->GetActorForwardVector();
				AlreadyHitActors.Add(LastHit.GetActor());

				const float MonsterATK = GetCurrentStatValue.Execute(EStats::ATK);
				const float CalcATK = CloseAttackCorrectionValue * MonsterATK;

				UGameplayStatics::ApplyPointDamage(LastHit.GetActor(), CalcATK, HitFromDirection, LastHit, GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());

				ApplyImpact(LastHit.GetActor());
			}
		}
	}
}