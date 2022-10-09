#include "PickupActor.h"
#include "ToughSword.h"
#include "BaseWeapon.h"
#include "MeleeCharacter.h"
#include "Components/SphereComponent.h"

APickupActor::APickupActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Mesh;
	Mesh->SetupAttachment(RootComponent);
	Sphere->SetupAttachment(RootComponent);

	Sphere->SetSphereRadius(128.f);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube"));
    if(Cube.Succeeded())
    {
        Mesh->SetStaticMesh(Cube.Object);
    }
}

void APickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickupActor::Interact(AActor* Caller)
{
	UWorld* World = GetWorld();
	AMeleeCharacter* Character = Cast<AMeleeCharacter>(Caller);
	if(World && SpawnActor)
	{
		FActorSpawnParameters Params; 
		Params.Owner = Caller;
		Params.Instigator = Cast<APawn>(Caller);

		ABaseWeapon* Weapon = World->SpawnActor<ABaseWeapon>(SpawnActor, GetActorTransform(), Params);
		if(Weapon && Character)
		{
			Character->Equip(Weapon);
			//Weapon->OnEquipped();
		}
	}
}

