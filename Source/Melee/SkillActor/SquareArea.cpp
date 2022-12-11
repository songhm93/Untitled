#include "SquareArea.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "../Interface/CombatInterface.h"

ASquareArea::ASquareArea()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = BoxComp;
	

	bOvelapped = false;
	DamageTime = 0.f;
	IsMove = false;
}

void ASquareArea::BeginPlay()
{
	Super::BeginPlay();

	if(BoxComp)
	{
		BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OverlappedApplyDamage);
		BoxComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::RemoveOverlapped);
	}
	
	CurrentLoc = GetActorLocation();
	DestroyTime = 0.f;
}

void ASquareArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bOvelapped)
	{
		//시간을 0.5초마다 대미지 적용
		DamageTime += DeltaTime;
		if(DamageTime >= 0.5f)
		{
			//대미지 적용
			if(!DamageActor.IsEmpty())
			{
				for(auto DamagedActor : DamageActor)
				{
					UGameplayStatics::ApplyDamage(DamagedActor, 1000.f, Cast<APawn>(GetOwner())->GetController(), GetOwner(), UDamageType::StaticClass());
				}
				
			}
			DamageTime = 0.f;
		}
	}

	if(IsMove)
	{
		DestroyTime += DeltaTime;
		
		if(DestroyTime < 1.f)
		{
			SetActorLocation(FVector(CurrentLoc.X, CurrentLoc.Y, CurrentLoc.Z + 30.f));
		}
		else if(DestroyTime >= 5.f)
		{
			FVector Loc = FMath::VInterpTo(GetActorLocation(), FVector(CurrentLoc.X, CurrentLoc.Y, CurrentLoc.Z - 50.f), DeltaTime, 0.5f);
			SetActorLocation(Loc);
		}
		if(DestroyTime >= 15.f)
		{
			Destroy();
		}
	}

}

void ASquareArea::OverlappedApplyDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && OtherActor->Implements<UCombatInterface>())
	{
		bOvelapped = true;
		if(!DamageActor.Contains(OtherActor) && OtherActor != GetOwner()) //여기 getOwner부분 수정해야함
		{
			DamageActor.Add(OtherActor);
		}
	}
}

void ASquareArea::RemoveOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor && OtherActor->Implements<UCombatInterface>())
	{
		if(DamageActor.Contains(OtherActor))
		{
			DamageActor.Remove(OtherActor);
		}
	}
}

void ASquareArea::RequestDestroy()
{
	
	
	IsMove = true;
}