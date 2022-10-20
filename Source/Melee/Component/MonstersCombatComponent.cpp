#include "MonstersCombatComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "../PlayerController/EnemyAIController.h"

UMonstersCombatComponent::UMonstersCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bEnemyWeaponCollisionEnabled = false;
	CollisionMeshComponent = CreateDefaultSubobject<UPrimitiveComponent>(TEXT("CollisionMeshComponent"));

	RightWeaponStartSocketName = TEXT("RightWeaponStart");
	RightWeaponEndSocketName = TEXT("RightWeaponEnd");
	LeftWeaponStartSocketName = TEXT("LeftWeaponStart");
	LeftWeaponEndSocketName = TEXT("LeftWeaponEnd");
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
	} 
}


void UMonstersCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bEnemyWeaponCollisionEnabled)
		CollisionTrace();
}

void UMonstersCombatComponent::EnableCollision(bool bLeftWeapon) //true면 왼쪽무기.
{
	ClearHitActors();

	bEnemyWeaponCollisionEnabled = true;

	if (bLeftWeapon)
	{
		WeaponStartSocketName = LeftWeaponStartSocketName;
		WeaponEndSocketName = LeftWeaponEndSocketName;
	}
	else
	{
		WeaponStartSocketName = RightWeaponStartSocketName;
		WeaponEndSocketName = RightWeaponEndSocketName;
	}

}

void UMonstersCombatComponent::DisableCollision()
{
	bEnemyWeaponCollisionEnabled = false;
}

void UMonstersCombatComponent::CollisionTrace()
{
	if (nullptr == CollisionMeshComponent) return;
	const FVector Start = CollisionMeshComponent->GetSocketLocation(WeaponStartSocketName);
	const FVector End = CollisionMeshComponent->GetSocketLocation(WeaponEndSocketName);
	float TraceRadius = 30.f;

	TArray<TEnumAsByte<EObjectTypeQuery>> CollisionObjectType;
	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	CollisionObjectType.Add(Pawn);

	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(GetOwner());
	EDrawDebugTrace::Type DebugTrace = EDrawDebugTrace::ForDuration;
	TArray<FHitResult> OutHitResult;

	UKismetSystemLibrary::SphereTraceMultiForObjects(
		this,
		Start,
		End,
		TraceRadius,
		CollisionObjectType,
		false,
		ActorsToIgnore,
		DebugTrace,
		OutHitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Blue,
		5.f);

	if (!OutHitResult.IsEmpty())
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
			}
		}
	}
}

void UMonstersCombatComponent::ClearHitActors()
{
	AlreadyHitActors.Empty();
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
        CloseAttackCorrectionValue = 2.f;
        ReturnValue = TEXT("Second");
    }
    else if (RandomValue == 2)
    {
        CloseAttackCorrectionValue = 3.f;
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