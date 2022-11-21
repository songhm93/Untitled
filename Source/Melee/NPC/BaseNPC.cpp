#include "BaseNPC.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

ABaseNPC::ABaseNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	AreaSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphereComp"));
	InteractWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidgetComp"));
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	RootComponent = SkeletalMeshComp;
	AreaSphereComp->SetupAttachment(RootComponent);
	InteractWidgetComp->SetupAttachment(RootComponent);
}

void ABaseNPC::BeginPlay()
{
	Super::BeginPlay();
	
	AreaSphereComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::SphereBeginOverlap);
	AreaSphereComp->OnComponentEndOverlap.AddDynamic(this, &ThisClass::SphereEndOverlap);


	if(InteractWidgetComp)
	{
		InteractWidgetComp->SetVisibility(false);
	}		
}

void ABaseNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseNPC::Interact(AActor* Caller)
{
	if(InteractWidgetComp)
	{
		InteractWidgetComp->SetVisibility(false);
	}
	
}

void ABaseNPC::SphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(Cast<ACharacter>(OtherActor))
	{
		if(InteractWidgetComp)
		{
			InteractWidgetComp->SetVisibility(true);
		}
	}
	
}

void ABaseNPC::SphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(Cast<ACharacter>(OtherActor))
	{
		if(InteractWidgetComp)
		{
			InteractWidgetComp->SetVisibility(false);
		}
	}
}