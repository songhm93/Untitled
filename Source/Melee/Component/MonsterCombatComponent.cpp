#include "MonsterCombatComponent.h"

#include "Kismet/KismetSystemLibrary.h"

UMonsterCombatComponent::UMonsterCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bEnemyWeaponCollisionEnabled = false;
	CollisionMeshComponent = CreateDefaultSubobject<UPrimitiveComponent>(TEXT("CollisionMeshComponent"));

	RightWeaponStartSocketName = TEXT("RightWeaponStart");
	RightWeaponEndSocketName = TEXT("RightWeaponEnd");
	LeftWeaponStartSocketName = TEXT("LeftWeaponStart");
	LeftWeaponEndSocketName = TEXT("LeftWeaponEnd");
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
	float TraceRadius = 20.f;

	TArray<TEnumAsByte<EObjectTypeQuery>> CollisionObjectType;
	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	CollisionObjectType.Add(Pawn);

	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(GetOwner());
	EDrawDebugTrace::Type DebugTrace = EDrawDebugTrace::None;
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

	//여기에서 충돌한 애들 대미지 주는거, 델리게이트로 주는게 나을지도?
	
}

void UMonsterCombatComponent::ClearHitActors()
{
	AlreadyHitActors.Empty();
}
