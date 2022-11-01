#include "Portal.h"

APortal::APortal()
{
	PrimaryActorTick.bCanEverTick = false;
	SM1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM1"));
	SM2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM2"));
	SM1->SetupAttachment(RootComponent);
	SM2->SetupAttachment(SM1);

}

void APortal::BeginPlay()
{
	Super::BeginPlay();
	
}

void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortal::Interact(AActor* Caller)
{
	
}

