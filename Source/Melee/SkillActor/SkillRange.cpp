#include "SkillRange.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"



ASkillRange::ASkillRange()
{
	PrimaryActorTick.bCanEverTick = false;

	Skill4DestoryTime = 2.f;

}

void ASkillRange::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(Skill4DestroyTimerHandle, this, &ThisClass::Destroyed, Skill4DestoryTime);

}

void ASkillRange::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASkillRange::Destroyed()
{
	if(ImpactParticle && ImpactSound)
	{
		UGameplayStatics::ApplyRadialDamage(this, 2000.f, GetActorLocation() + FVector(0.f, 0.f, 20.f), 256.f, UDamageType::StaticClass(), TArray<AActor*>(), this, GetOwner()->GetInstigatorController());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorTransform());
		UGameplayStatics::SpawnSoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	Destroy();
}
