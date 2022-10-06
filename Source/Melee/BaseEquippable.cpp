#include "BaseEquippable.h"
#include "GameFramework/Character.h"
#include "Interactable.h"

ABaseEquippable::ABaseEquippable()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemSkeletalMesh"));
	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMesh"));
	RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("EmRootSceneCompptyComp"));
	RootComponent = RootSceneComp;
	ItemSkeletalMesh->SetupAttachment(RootComponent);
	ItemStaticMesh->SetupAttachment(RootComponent);
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

UPrimitiveComponent* ABaseEquippable::GetItemMesh()
{
	if(ItemStaticMesh)
		return ItemStaticMesh;
	else 
		return ItemSkeletalMesh;
	
}

void ABaseEquippable::OnEquipped()
{
	bIsEquipped = true;
	AttachActor(AttachSocketName);
}

void ABaseEquippable::OnUnequipped()
{
	if(bIsEquipped)
		bIsEquipped = false;
}

void ABaseEquippable::AttachActor(FName SocketName)
{
	FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), Rules, SocketName);
	
}

void ABaseEquippable::Interact(AActor* Caller)
{
	
}