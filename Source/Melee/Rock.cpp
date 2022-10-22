#include "Rock.h"
#include "Components/CapsuleComponent.h"
//#include "GeometryCollection/GeometryCollectionComponent.h"

ARock::ARock()
{
	PrimaryActorTick.bCanEverTick = false;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetSimulatePhysics(false);
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	RootComponent = CapsuleComp;
	CapsuleComp->SetCapsuleHalfHeight(270.f);
	CapsuleComp->SetCapsuleRadius(120.f);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetSimulatePhysics(false);
	CapsuleComp->SetNotifyRigidBodyCollision(false);
	MeshComp->SetupAttachment(RootComponent);

	
	
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> RockMesh(TEXT("/Game/Migrated/SM_Rock_To_Hold"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> RockMesh(TEXT("/Game/CombatSystem/Blueprints/Monster/Rampage/SM_Rock_To_Hold1_GeometryCollection_SM"));
	if (RockMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(RockMesh.Object);
	}
}

void ARock::BeginPlay()
{
	Super::BeginPlay();

	CapsuleComp->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
}

void ARock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARock::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//캐릭터면 대미지 입히고,
	//바위는 파괴.
}
