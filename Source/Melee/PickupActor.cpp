#include "PickupActor.h"
#include "ToughSword.h"
#include "BaseWeapon.h"
#include "MeleeCharacter.h"

APickupActor::APickupActor()
{
	PrimaryActorTick.bCanEverTick = true;

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
	if(World && SpawnActor)
	{
		FActorSpawnParameters Params; 
		Params.Owner = Caller;
		Params.Instigator = Cast<APawn>(Caller);
		ABaseWeapon* Weapon = World->SpawnActor<ABaseWeapon>(SpawnActor, GetActorTransform(), Params);
		if(Weapon)
		{
			Weapon->OnEquipped();
		}
	}
}

