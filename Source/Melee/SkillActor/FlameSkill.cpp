#include "FlameSkill.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

#include "../Interface/CombatInterface.h"

AFlameSkill::AFlameSkill()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(RootComponent);
	SphereComp->SetSphereRadius(64.f);
	bOvelapped = false;
	DestroyFlameTime = 1.5f;
	DamageTime = 0.f;
}

void AFlameSkill::BeginPlay()
{
	Super::BeginPlay();
	if(SphereComp)
	{
		SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OverlappedApplyDamage);
		SphereComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::RemoveOverlapped);
	}
	GetWorld()->GetTimerManager().SetTimer(DestoryFlameTimerHandle, this, &ThisClass::DestroyFlame, DestroyFlameTime);
}

void AFlameSkill::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bOvelapped)
	{
		//시간을 0.25초마다 대미지 적용
		DamageTime += DeltaTime;
		if(DamageTime >= 0.25f)
		{
			//여기서 대미지 적용
			if(!DamageActor.IsEmpty())
			{
				for(auto DamagedActor : DamageActor)
				{
					UGameplayStatics::ApplyDamage(DamagedActor, 3.5f, Cast<APawn>(GetOwner())->GetController(), this, UDamageType::StaticClass());
				}
				
			}
			DamageTime = 0.f;
		}
	}

}

void AFlameSkill::OverlappedApplyDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor->Implements<UCombatInterface>())
	{
		bOvelapped = true;
		if(!DamageActor.Contains(OtherActor) && OtherActor != GetOwner())
		{
			DamageActor.Add(OtherActor);
		}
	}
}

void AFlameSkill::DestroyFlame()
{
	Destroy();
}

void AFlameSkill::RemoveOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor && OtherActor->Implements<UCombatInterface>())
	{
		if(DamageActor.Contains(OtherActor))
		{
			DamageActor.Remove(OtherActor);
		}
	}
}
