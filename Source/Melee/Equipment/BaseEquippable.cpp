#include "BaseEquippable.h"

#include "GameFramework/Character.h"

#include "../Interface/Interactable.h"

ABaseEquippable::ABaseEquippable()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemStaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMeshComp"));
	RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("EmRootSceneCompptyComp"));
	RootComponent = RootSceneComp;
	ItemStaticMeshComp->SetupAttachment(RootComponent);
	AttachSocketName = TEXT("SwordHipAttachSocket");
	
}

void ABaseEquippable::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseEquippable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UPrimitiveComponent* ABaseEquippable::GetItemMeshComp()
{
	if(ItemStaticMeshComp)
		return ItemStaticMeshComp;
		
	return nullptr;
}

void ABaseEquippable::Interact(AActor* Caller)
{
	
}