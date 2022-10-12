#include "CollisionComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../BaseWeapon.h"

UCollisionComponent::UCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	bIsCollisionEnabled = false;
	CollisionMeshComponent = CreateDefaultSubobject<UPrimitiveComponent>(TEXT("CollisionMeshComponent"));

	StartSocketName = TEXT("WeaponStart"); //무기 메시에 소켓 추가
	EndSocketName = TEXT("WeaponEnd");
}


void UCollisionComponent::BeginPlay()
{
	Super::BeginPlay();

	Weapon = Cast<ABaseWeapon>(GetOwner());
}

void UCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bIsCollisionEnabled)
	{
		CollisionTrace();
	}

}

void UCollisionComponent::EnableCollision()
{
	ClearHitActors();
	bIsCollisionEnabled = true;
}

void UCollisionComponent::DisableCollision()
{
	bIsCollisionEnabled = false;
}

void UCollisionComponent::CollisionTrace()
{	
	
	const FVector Start = CollisionMeshComponent->GetSocketLocation(StartSocketName);
	const FVector End = CollisionMeshComponent->GetSocketLocation(EndSocketName);
	float TraceRadius = 20.f;

	TArray<TEnumAsByte<EObjectTypeQuery>> CollisionObjectType;
	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	CollisionObjectType.Add(Pawn);

	TArray<AActor*> ActorsToIgnore;
	if(Weapon)
		ActorsToIgnore.Add(Weapon->GetOwner());
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

	if(Weapon)
	{
		for(auto LastHit : OutHitResult)
		{
			if(!AlreadyHitActors.Contains(LastHit.GetActor()))
			{
				AlreadyHitActors.Add(LastHit.GetActor());
				Weapon->OnHit(LastHit);
			}
		}
	}
	
}

void UCollisionComponent::ClearHitActors()
{
	AlreadyHitActors.Empty();
}

