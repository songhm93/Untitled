#include "MonsterCombatComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

UMonsterCombatComponent::UMonsterCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bEnemyWeaponCollisionEnabled = false;
	CollisionMeshComponent = CreateDefaultSubobject<UPrimitiveComponent>(TEXT("CollisionMeshComponent"));

	RightWeaponStartSocketName = TEXT("RightWeaponStart");
	RightWeaponEndSocketName = TEXT("RightWeaponEnd");
	LeftWeaponStartSocketName = TEXT("LeftWeaponStart");
	LeftWeaponEndSocketName = TEXT("LeftWeaponEnd");
	CloseAttackCorrectionValue = 1.f;
}


void UMonsterCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UMonsterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bEnemyWeaponCollisionEnabled)
		CollisionTrace();
}

void UMonsterCombatComponent::EnableCollision(bool bLeftWeapon) //true면 왼쪽무기.
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

void UMonsterCombatComponent::DisableCollision()
{
	bEnemyWeaponCollisionEnabled = false;
}

void UMonsterCombatComponent::CollisionTrace()
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
				const FVector HitFromDirection = GetOwner()->GetActorForwardVector();
				AlreadyHitActors.Add(LastHit.GetActor());

				const float MonsterATK = GetCurrentStatValue.Execute(EStats::ATK);
				const float CalcATK = CloseAttackCorrectionValue * MonsterATK;

				UGameplayStatics::ApplyPointDamage(LastHit.GetActor(), CalcATK, HitFromDirection, LastHit, GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
			}
		}
	}
}

void UMonsterCombatComponent::ClearHitActors()
{
	AlreadyHitActors.Empty();
}
