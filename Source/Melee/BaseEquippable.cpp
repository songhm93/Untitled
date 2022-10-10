#include "BaseEquippable.h"
#include "GameFramework/Character.h"
#include "Interactable.h"

ABaseEquippable::ABaseEquippable()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemSkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemSkeletalMeshComp"));
	ItemStaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMeshComp"));
	RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("EmRootSceneCompptyComp"));
	RootComponent = RootSceneComp;
	ItemSkeletalMeshComp->SetupAttachment(RootComponent);
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
	else 
		return ItemSkeletalMeshComp;
	
}

void ABaseEquippable::Interact(AActor* Caller)
{
	
}