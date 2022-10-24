#include "SkillRange.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"



ASkillRange::ASkillRange()
{
 	
	PrimaryActorTick.bCanEverTick = true;



}

void ASkillRange::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASkillRange::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASkillRange::Destroyed()
{
	if(ImpactParticle && ImpactSound)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorTransform());
		UGameplayStatics::SpawnSoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	
}
