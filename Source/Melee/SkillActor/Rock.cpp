#include "Rock.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

#include "../PlayerCharacter/BaseCharacter.h"
#include "../MonsterCharacter/EnemyCharacter.h"
#include "../AttackDamageType.h"


ARock::ARock()
{
	PrimaryActorTick.bCanEverTick = true;
	
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

	Damage = 2000.f;

	SpawnTime = 0.f;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> RockMesh(TEXT("/Game/Migrated/SM_Rock_To_Hold"));
	
	if (RockMesh.Succeeded())
	{
		MeshComp->SetStaticMesh(RockMesh.Object);
	}
}

void ARock::BeginPlay()
{
	Super::BeginPlay();

	if(CapsuleComp)
	{
		CapsuleComp->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	}
	
}

void ARock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpawnTime+= DeltaTime;

	if(SpawnTime > 3.f)
		HitEffect();

}

void ARock::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//캐릭터면 대미지 입히고,
	//바위는 파괴.

	if(OtherActor)
	{	
		ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(OtherActor);
		if(PlayerCharacter)
		{
			AController* InstigatorController = Cast<APawn>(GetOwner())->GetController();
			if(InstigatorController)
			{
				UGameplayStatics::ApplyRadialDamageWithFalloff(
					this, 
					Damage, 
					1500.f, //최소 데미지
					GetActorLocation(), //데미지 반경의 원점
					200.f, //데미지 내부 반경
					500.f, //데미지 외부 반경
					1.f, //데미지 감소
					DamageType, //DamageType 클래스
					TArray<AActor*>(), //무시할 액터
					this, // 데미지를 입힌 액터
					InstigatorController 
				);
			}
		}
		HitEffect();
	}
}

void ARock::HitEffect()
{
	if(ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorTransform());
	}
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	Destroy();
}

void ARock::Destroyed()
{
	if(Cast<AEnemyCharacter>(GetOwner()))
	{
		Cast<AEnemyCharacter>(GetOwner())->PlayCameraShake(false);
	}
	
	if(ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorTransform());
	}
	if(ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
}
